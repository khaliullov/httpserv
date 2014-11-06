#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <cstdint>
#include <cstring>

#include <vector>
#include <stdexcept>
#include <system_error>
#include <memory>
#include <string>

#include "net_error.h"

namespace net {


class address
{
	struct FreeInfoFunctor {
		void operator () (struct addrinfo * p) { freeaddrinfo(p); }
	};

	typedef std::unique_ptr<struct addrinfo, FreeInfoFunctor> address_list;

 public:
	address(const std::string & address, const std::string & service)
	  : m_sockaddrlen(0)
	  , m_sockaddr()
	  , m_address(address)
	  , m_service(service)
	{ }

	address(const sockaddr * addr, socklen_t addrlen)
	  : m_sockaddrlen(addrlen)
	  , m_sockaddr(m_sockaddrlen ? new char[m_sockaddrlen] : nullptr)
	  , m_address()
	  , m_service()
	{
		memcpy(m_sockaddr.get(), addr, addrlen);
	}

	address(const address & other)
	  : m_sockaddrlen(other.m_sockaddrlen)
	  , m_sockaddr(m_sockaddrlen ? new char[m_sockaddrlen] : nullptr)
	  , m_address(other.m_address)
	  , m_service(other.m_service)
	{
		memcpy(m_sockaddr.get(), other.m_sockaddr.get(), m_sockaddrlen);
	}

	~address()
	{ }

	void resolve_sock_addr(int flags = (NI_NUMERICSERV))
	{
		const int buflen = 80;
		char buffer1[80], buffer2[80];

		int rc = getnameinfo(reinterpret_cast<struct sockaddr*>(m_sockaddr.get()),
		                     m_sockaddrlen, buffer1, buflen,
		                     buffer2, buflen, flags);

		if (rc) throw net::make_ainfo_error(rc, __func__);
		m_address = buffer1;
		m_service = buffer2;
	}

	const sockaddr * addr() const
	{ return reinterpret_cast<struct sockaddr*>(m_sockaddr.get()); }

	socklen_t addrlen() const
	{ return m_sockaddrlen; }

	void print()
	{
		if (m_address.empty() || m_service.empty())
			resolve_sock_addr();

		printf("%s:%s\n", m_address.c_str(), m_service.c_str());
	}

	static std::vector<address>
	list_passive_addresses(const int type,
	                       const std::string & portNumber)
	{
		std::vector<address> retlist;
		int rc = 0;

		struct addrinfo * tmp = nullptr;
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = type;
		hints.ai_flags = AI_PASSIVE;

		if ((rc = getaddrinfo(nullptr, portNumber.c_str(), &hints, &tmp)) != 0)
			throw net::make_ainfo_error(rc, __func__);

		address_list info(tmp);

		for (tmp = info.get(); tmp != nullptr; tmp = tmp->ai_next)
		{
			retlist.emplace_back(tmp->ai_addr, tmp->ai_addrlen);
		}
		return retlist;
	}

 protected:
	socklen_t m_sockaddrlen;
	std::unique_ptr<char[]> m_sockaddr;
	std::string m_address;
	std::string m_service;
};

//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////////////////
int main(int argc, char ** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		printf("%s\n", argv[i]);
		auto list =
			net::address::list_passive_addresses(SOCK_STREAM, argv[i]);

		for (auto i : list)
		{
			i.print();
		}
	}
}
