#include <unistd.h>

#include <cstdio>
#include <vector>
#include <functional>
#include <thread>

#include "serverconfig.h"
#include "tcp_server_socket.h"
#include "event.h"
#include "interactive_listener.h"

//////////////////////////////////////////////////////////////////////
struct server_socket_listener : public event_listener
{
	server_socket_listener(net::tcp_server_socket && sock)
	  : server_sock(std::move(sock))
		{ }

	virtual ~server_socket_listener()
		{ }

	void action(event_loop & evloop, uint32_t events) override
	{
		printf("%s: mask %s\n", __func__,
		       mask_to_string(events).c_str());

		if (events & EPOLLIN)
		{
			std::string assembled; // XXX horrible FIXME
			char buff[1024];
			ssize_t n = 0;
			do {
				n = read(server_sock, buff, 1024);

				printf("-> read() returned %zd bytes\n", n);

				if (n > 0)
				{
					assembled.append(buff, n);

					if (n < 1024)
						break;
				} else if (n < 0)
				{
					if (!(server_sock.is_nonblocking() && errno == EAGAIN))
						throw make_syserr("Could not read from descriptor");
				} else // n == 0
				{
//					printf("End of file signaled with read return of 0; "
//					       "event mask %s RDHUP bit set\n",
//					       (events & EPOLLRDHUP) ? "has" : "does not have");

//					if ((events & EPOLLRDHUP) == 0)
//						throw std::logic_error("Unexpected event mask");
				}
			} while (n > 0);

			printf("-> Read string '''%s'''\n", assembled.c_str());
		}

		if (events & EPOLLRDHUP)
		{
			printf("Client closed connection\n");
			evloop.delete_event(this->get_shared());
		}
	}

	int get_descriptor() { return server_sock; }

	uint32_t get_default_events() const override
		{ return all_et_input_events; }

	int descriptor() const override
		{ return server_sock; }

	net::tcp_server_socket server_sock;
};

//////////////////////////////////////////////////////////////////////
struct connection_listener : public event_listener
{
	connection_listener(net::tcp_listening_socket && sock)
	  : listen_sock(std::move(sock))
		{ }

	virtual ~connection_listener()
		{ }

	void action(event_loop & evloop, uint32_t events) override
	{
		std::unique_ptr<net::tcp_server_socket> ss_ptr;

		printf("%s: event mask = %s\n", __func__,
		       mask_to_string(events).c_str());

		do {
			ss_ptr = listen_sock.accept_alt<net::tcp_server_socket>();

			if (ss_ptr)
			{
				printf("Accepted client connection from %s - fd = %d, "
				       "server fd = %d\n",
				       ss_ptr->client_address().str().c_str(),
				       (int) *ss_ptr, (int) listen_sock);

				std::shared_ptr<event_listener> listener(
				  new server_socket_listener(std::move(*ss_ptr)));

				evloop.add_event(listener);
			}
		} while (listen_sock.is_nonblocking() && ss_ptr);
	}

	uint32_t get_default_events() const override
		{ return (EPOLLIN | EPOLLET); }

	int descriptor() const override
		{ return listen_sock; }

	net::tcp_listening_socket listen_sock;
};

void wrapper(event_loop & loop, int instance)
{
	try {
		loop.run(instance);
	} catch (std::exception & e)
	{
		printf("Thread %d - caught exception: %s\n", instance,  e.what());
	}
}

//////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	try { 
		std::vector<std::thread> selectors;

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

		printf("FOOBAR\n");
		for (int i = 0; i < 20; ++i)
		{
			printf("RAWR %d\n", i);
			selectors.emplace_back(wrapper, std::ref(evloop), i);
		}

		for (auto & t : selectors)
			t.join();
//		evloop.run();
	} catch (std::exception & e)
	{
		printf("Caught exception: %s\n", e.what());
	}

	return 0;
}
