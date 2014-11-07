#include <unistd.h>
#include <sys/socket.h>
#include <cstdio>
#include <vector>

#include "utility/util.h"
#include "serverconfig.h"

int main(int argc, char ** argv)
{
	server_config config(argc, argv);
	std::vector<int> serverFds;

	for (auto & addr : config.server_address_list)
	{
		int sfd = socket(addr.get_addr()->sa_family, SOCK_STREAM, 0);

		printf("Server listening on address %s\n", addr.str().c_str());

		if (  addr.get_addr()->sa_family == 10
		   && config.server_address_list.size() != 1)
		{
			int x = 1;
			setsockopt(sfd, IPPROTO_IPV6, IPV6_V6ONLY, &x, sizeof(x));
		}

		if (sfd < 0)
			throw make_syserr("socket() failed");

		if (bind(sfd, addr.get_addr(), addr.get_addrlen()) != 0)
			throw make_syserr("bind() failed");

		if (listen(sfd, 5) != 0)
			throw make_syserr("listen() failed");

		serverFds.push_back(sfd);
	}

	return 0;
}
