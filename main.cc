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

//////////////////////////////////////////////////////////////////////
struct connection_listener : public event_listener
{
	connection_listener(net::tcp_listening_socket && sock)
	  : listen_sock(std::move(sock))
		{ }

	virtual ~connection_listener()
		{ }

	void action(event_loop &, uint32_t events) override
	{
		std::shared_ptr<net::tcp_server_socket> ss_ptr;

		printf("event mask = %08x\n", events);

		do {
			ss_ptr = listen_sock.accept_alt<net::tcp_server_socket>();

			if (ss_ptr)
			{
				printf("Accepted client connection from %s - fd = %d, "
				       "server fd = %d\n",
				       ss_ptr->client_address().str().c_str(),
				       (int) *ss_ptr, (int) listen_sock);

			}
		} while (listen_sock.is_nonblocking() && ss_ptr);
	}

	uint32_t get_default_events() const override
		{ return (EPOLLIN | EPOLLET); }

	int descriptor() const override
		{ return listen_sock; }

	net::tcp_listening_socket listen_sock;
};

//////////////////////////////////////////////////////////////////////
struct server_socket_listener : public event_listener
{
	server_socket_listener(net::tcp_server_socket && sock)
	  : server_sock(std::move(sock))
		{ }

	virtual ~server_socket_listener()
		{ }

	void action(event_loop &, uint32_t events) override
	{
		printf("%s: got event mask %08x\n", __PRETTY_FUNCTION__, events);
	}

	uint32_t get_default_events() const override
		{ return 0; }

	int descriptor() const override
		{ return server_sock; }

	net::tcp_server_socket server_sock;
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
