#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <vector>

#include "utility/util.h"
#include "serverconfig.h"
#include "tcp_server_socket.h"

//const int all_events = (EPOLLIN | EPOLLOUT | 
//                        EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP);

int main(int argc, char ** argv)
{
	server_config config(argc, argv);
	std::vector<net::tcp_server_socket> serverFds;
	int epfd = -1;

	serverFds.reserve(config.server_address_list.size());

	epfd = epoll_create1(EPOLL_CLOEXEC);

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
		ev.events = EPOLLIN;
		ev.data.ptr = &serverFds.back();

		epoll_ctl(epfd, EPOLL_CTL_ADD, serverFds.back(),  &ev);
	}

	
	const size_t max_events = 128;
	struct epoll_event ev_list[max_events];

	int count = 0;
	while ((count = epoll_wait(epfd, ev_list, max_events, -1)) > 0)
	{
		printf("Got %d events\n", count);

		for (int i = 0; i < count; ++i)
		{
			int fd = -1;
			net::address addr;
			net::tcp_server_socket * active =
			  reinterpret_cast<net::tcp_server_socket*>(ev_list[i].data.ptr);
			printf("event mask = %08x\n", ev_list[i].events);

			std::tie(fd, addr) = active->accept(SOCK_NONBLOCK|SOCK_CLOEXEC);

			printf("Accepted client connection from %s - fd = %d\n",
			       addr.str().c_str(), fd);

			close(fd);
		}

		break;
	}

	return 0;
}
