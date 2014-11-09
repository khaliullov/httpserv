#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <vector>

#include "utility/util.h"
#include "serverconfig.h"
#include "tcp_server_socket.h"

#define MYDEBUG 1

//const int all_events = (EPOLLIN | EPOLLOUT | 
//                        EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP);

struct event
{
	event() { }
	virtual ~event() { }

	event(const event &) = delete;
	event(event &&); 

	event & operator = (const event &) = delete;
	event & operator = (event &&)
	{ return *this; }

#if MYDEBUG
	virtual void action(uint32_t events)
	{ printf ("base action called mask = %08x\n", events); }
#else
	virtual void action(uint32_t) = 0;
#endif
};

//////////////////////////////////////////////////////////////////////
struct connection_event : public event
{
	connection_event (net::tcp_server_socket && sock)
	  : server_sock(std::move(sock))
	{ }

	virtual ~connection_event () { }

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
		} while (fd >= 0);
	}

	net::tcp_server_socket server_sock;
};

namespace net {
	typedef int client_socket;
}

struct client_event : public event
{
	client_event(net::client_socket && sock)
	;

	virtual ~client_event();

	virtual void action(uint32_t events) override;

	client_socket client_sock;
};

//////////////////////////////////////////////////////////////////////
class event_loop
{
 public:
	event_loop(int events = 128, int flags = EPOLL_CLOEXEC)
	  : evfd(epoll_create1(flags))
	  , max_events(events)
	  , ev_list(new epoll_event[events])
	{
		if (evfd < 0)
			throw make_syserr("epoll_create() failed");
	}

	~event_loop()
	{
		if (evfd >= 0) close(evfd);
	}

	void add_event(event && e);
	void delete_event();
	
	void run()
	{
		int count = 0;
		while ((count = epoll_wait(evfd, ev_list.get(), max_events, -1)) > 0)
		{
			for (int i = 0; i < count; ++i)
			{
				event * e = reinterpret_cast<event*>(ev_list[i].data.ptr);
				e->action(ev_list[i].events);
			}
		}
	}

 private:
	int evfd;
	int max_events;
	std::unique_ptr<struct epoll_event[]> ev_list;
};

//////////////////////////////////////////////////////////////////////
void loop(int evfd)
{
	const size_t max_events = 128;
	struct epoll_event ev_list[max_events];

	int count = 0;
	while ((count = epoll_wait(evfd, ev_list, max_events, -1)) > 0)
	{
		printf("Got %d events\n", count);

		for (int i = 0; i < count; ++i)
		{
			net::tcp_server_socket * active =
			  reinterpret_cast<net::tcp_server_socket*>(ev_list[i].data.ptr);

			int fd = -1;
			net::address addr;

			printf("event mask = %08x\n", ev_list[i].events);

			do {
				std::tie(fd, addr) = active->accept();

				if (fd >= 0)
				{
					printf("Accepted client connection from %s - fd = %d, "
					       "server fd = %d\n",
					       addr.str().c_str(), fd, (int) *active);

					close(fd);
				}
			} while (fd >= 0);
		}

		printf("calling epoll_wait\n");

	}
}

int main(int argc, char ** argv)
{
	server_config config(argc, argv);
	std::vector<net::tcp_server_socket> serverFds;
	int epfd = -1;

	serverFds.reserve(config.server_address_list.size());

	epfd = epoll_create1(EPOLL_CLOEXEC);
	event_loop evloop;

	for (auto & addr : config.server_address_list)
	{
		net::socket_option_list opts;
		opts.emplace_back(SOL_SOCKET, SO_REUSEADDR, 1);

		printf("Server listening on address %s\n", addr.str().c_str());

		if (  addr.get_addr()->sa_family == AF_INET6
		   && config.server_address_list.size() != 1)
		{
			opts.emplace_back(IPPROTO_IPV6, IPV6_V6ONLY, 1);
			serverFds.emplace_back(addr, opts);
		} else
		{
			serverFds.emplace_back(addr);
		}

		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.events = EPOLLIN | EPOLLET;
		ev.data.ptr = &serverFds.back();

		epoll_ctl(epfd, EPOLL_CTL_ADD, serverFds.back(),  &ev);
	}

	loop(epfd);

	return 0;
}
