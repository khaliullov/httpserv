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
#include "event.h"
#include "interactive_listener.h"

#define MYDEBUG 1

//////////////////////////////////////////////////////////////////////
struct connection_listener : public event_listener
{
	connection_listener(net::tcp_listening_socket && sock)
	  : server_sock(std::move(sock))
	{ }

	virtual ~connection_listener() { }

	virtual void action(event_loop &, uint32_t events) override
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

	net::tcp_listening_socket server_sock;
};


//////////////////////////////////////////////////////////////////////
namespace net {
	typedef int client_socket;
}

struct client_event : public event_listener
{
	client_event(net::client_socket && sock) ;

	virtual ~client_event();

	void action(event_loop &, uint32_t events) override;

	net::client_socket client_sock;
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
			                net::tcp_listening_socket(addr, opts)));
			evloop.add_event(ptr);
		}

		std::shared_ptr<event_listener> inter(
			new interactive_listener(STDIN_FILENO));

		evloop.add_event(inter);

		evloop.run();
	} catch (std::exception & e)
	{
		printf("Caught exception: %s\n", e.what());
	}

	return 0;
}
