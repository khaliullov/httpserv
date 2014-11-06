#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <utility/util.h>

#include "serverconfig.h"

int main(int argc, char ** argv)
{
	server_config config(argc, argv);

	for (auto & addr : config.server_address_list)
	{
		addr.print();

		printf("--> family = %d\n", addr.addr()->sa_family);
		int sfd = socket(addr.addr()->sa_family, SOCK_STREAM, 0);

		if (addr.addr()->sa_family == 10)
		{
			int x = 1;
			setsockopt(sfd, IPPROTO_IPV6, IPV6_V6ONLY, &x, sizeof(x));
		}

		if (sfd < 0)
			throw make_syserr("socket() failed");

		if (bind(sfd, addr.addr(), addr.addrlen()) != 0)
			throw make_syserr("bind() failed");


		if (listen(sfd, 5) != 0)
			throw make_syserr("listen() failed");
	}
	sleep(120);

	return 0;
}
