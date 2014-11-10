#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <set>
#include <vector>

#include <readline/readline.h>

#include "utility/util.h"
#include "serverconfig.h"
#include "tcp_server_socket.h"

#define MYDEBUG 1

//const int all_events = (EPOLLIN | EPOLLOUT | 
//                        EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP);

//////////////////////////////////////////////////////////////////////
struct event_listener
{
	event_listener() { }

	virtual ~event_listener() { }

	event_listener(const event_listener &) = delete;

	event_listener & operator = (const event_listener &) = delete;

	event_listener(event_listener &&) { }

	event_listener & operator = (event_listener &&) { return *this; }

	virtual void action(uint32_t events) = 0;

	virtual uint32_t get_default_events() const = 0;

	virtual int descriptor() const = 0;
};

//////////////////////////////////////////////////////////////////////
struct connection_listener : public event_listener
{
	connection_listener(net::tcp_server_socket && sock)
	  : server_sock(std::move(sock))
	{ }

	virtual ~connection_listener() { }

	virtual void action(uint32_t events) override
	{
		int fd = -1;
		net::address addr;

		printf("event mask = %08x\n", events);

		do {
			std::tie(fd, addr) = server_sock.accept();

			if (fd >= 0)
			{
				printf("Accepted client connection from %s - fd = %d, "
				       "server fd = %d\n",
				       addr.str().c_str(), fd, (int) server_sock);

				close(fd);
			}
		} while (server_sock.is_nonblocking() && fd >= 0);
	}

	virtual uint32_t get_default_events() const
		{ return (EPOLLIN | EPOLLET); }

	int descriptor() const override
		{ return server_sock; }

	net::tcp_server_socket server_sock;
};

extern "C" void readlineCB(char * s)
{
	printf("READ CMD %s\n", s);
	std::string cmd(s);
	free(s);
	if (cmd == "quit")
		throw std::runtime_error("quiting");
}

struct interative_listener : public event_listener
{
	interative_listener(int input_fd)
	  : infd(input_fd)
	{
		rl_callback_handler_install("statserv> ", readlineCB);
	}

	virtual ~interative_listener()
	{
		rl_callback_handler_remove();
		close(infd);
	}

	virtual void action(uint32_t events)
	{
		if (events & EPOLLIN)
		{
			rl_callback_read_char();
		}
	}

	virtual uint32_t get_default_events() const
		{ return (EPOLLIN); }

	int descriptor() const override
		{ return infd; }

	int infd;
};

namespace net {
	typedef int client_socket;
}

struct client_event : public event_listener
{
	client_event(net::client_socket && sock) ;

	virtual ~client_event();

	void action(uint32_t events) override;

	net::client_socket client_sock;
};

//////////////////////////////////////////////////////////////////////
class event_loop
{
 public:
	event_loop(int events = 128, int flags = EPOLL_CLOEXEC)
	  : evfd(epoll_create1(flags))
	  , max_events(events)
	  , ev_buffer(new epoll_event[events])
		{ if (evfd < 0) throw make_syserr("epoll_create() failed"); }

	~event_loop()
		{ if (evfd >= 0) close(evfd); }

	void add_event(std::shared_ptr<event_listener> & e)
	{
		event_list.insert(e);

		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.events = e->get_default_events();
		ev.data.ptr = e.get();

		epoll_ctl(evfd, EPOLL_CTL_ADD, e->descriptor(),  &ev);
	}

	void delete_event();
	
	void run()
	{
		int count = 0;
		while ((count = epoll_wait(evfd, ev_buffer.get(), max_events, -1)) > 0)
		{
//			printf("%s: Read %d events\n", __func__, count);

			for (int i = 0; i < count; ++i)
			{
				event_listener * listener =
				  reinterpret_cast<event_listener*>(ev_buffer[i].data.ptr);

				listener->action(ev_buffer[i].events);
			}
//			printf("Waiting for event...\n");
		}
	}

 private:
	int evfd;

	std::set<std::shared_ptr<event_listener>> event_list;

	int max_events;
	std::unique_ptr<struct epoll_event[]> ev_buffer;
};

//////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	try { 
		server_config config(argc, argv);

		event_loop evloop;

		for (auto & addr : config.server_address_list)
		{
			net::socket_option_list opts;
			opts.emplace_back(SOL_SOCKET, SO_REUSEADDR, 1);

			printf("Server listening on address %s\n", addr.str().c_str());

			std::shared_ptr<event_listener> ptr;

			if (  addr.get_addr()->sa_family == AF_INET6
			   && config.server_address_list.size() != 1)
			{
				opts.emplace_back(IPPROTO_IPV6, IPV6_V6ONLY, 1);
			}

			ptr.reset(new connection_listener(
			                net::tcp_server_socket(addr, opts)));
			evloop.add_event(ptr);
		}

		std::shared_ptr<event_listener> inter(
			new interative_listener(STDIN_FILENO));
		evloop.add_event(inter);

		evloop.run();
	} catch (std::exception & e)
	{
		printf("Caught exception: %s\n", e.what());
	}

	return 0;
}
