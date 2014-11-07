#include <unistd.h>
#include <sys/socket.h>
#include <cstdio>
#include <vector>

#include "utility/util.h"
#include "serverconfig.h"
#include "tcp_server_socket.h"

int main(int argc, char ** argv)
{
	server_config config(argc, argv);
	std::vector<net::tcp_server_socket> serverFds;

	for (auto & addr : config.server_address_list)
	{
		printf("Server listening on address %s\n", addr.str().c_str());

		if (  addr.get_addr()->sa_family == AF_INET6
		   && config.server_address_list.size() != 1)
		{
			net::socket_option_list opts{ { IPPROTO_IPV6, IPV6_V6ONLY, 1 } };
			serverFds.emplace_back(addr, opts);
		} else
		{
			serverFds.emplace_back(addr);
		}
	}
	sleep(30);

	return 0;
}
