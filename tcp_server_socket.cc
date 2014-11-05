#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <cstdint>
#include <stdexcept>
#include <string>

//const char * addr = "0.0.0.0";
//const char * addr = "127.0.0.1";
const char * addr = "ip6-localhost";
uint16_t port = 81;

namespace net {

class address {
 public:
	address(const char * a, uint16_t )
	{
		int rc = 0;
		struct addrinfo * myinfo = nullptr;
		if ((rc = getaddrinfo(a, "81", nullptr, &myinfo)) != 0)
			throw std::runtime_error(gai_strerror(rc));

		int x = 0;
		for (struct addrinfo * p = myinfo; p != nullptr; p = p->ai_next)
		{
			printf("---> flags = %08x\n", p->ai_flags);
			printf("     family = %08x\n", p->ai_family);
			printf("     socktype = %08x\n", p->ai_socktype);
			printf("     protocol = %08x\n", p->ai_protocol);
			printf("     addrlen = %08x\n", p->ai_addrlen);

			const char * ap = reinterpret_cast<char *>(p->ai_addr);
			for (size_t i = 0; i < p->ai_addrlen; ++i)
				printf("%02hhx ", ap[i]);
			putchar('\n');

			printf("     canonname = %s\n", p->ai_canonname);


			++x;
		}

		printf("Got %d addresses\n", x);
	}
};

class socket
{
 public:
	socket(int domain, int type, int protocol)
	: fd(::socket(domain, type, protocol))
	{
		if (fd < 0) throw std::runtime_error("Could not create socket");
	}

	virtual ~socket() { close(fd); }

 protected:
	int fd;
};

class tcp_server_socket : net::socket {
 public:
	tcp_server_socket(const address & a)
	: socket(AF_INET, SOCK_STREAM, 0)
	, m_address(a)
	{
	}

 private:
	address m_address;
};

} // namespace net

int main()
{
	net::tcp_server_socket ss(net::address(addr, port));
}
