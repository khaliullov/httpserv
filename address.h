#ifndef GUARD_ADDRESS_H
#define GUARD_ADDRESS_H 1

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <cstring>
#include <vector>
#include <string>
#include <memory>

#include "net_error.h"

namespace net {

class address
{
 protected:
	struct FreeInfoFunctor {
		void operator () (struct addrinfo * p) { freeaddrinfo(p); }
	};

	typedef std::unique_ptr<struct addrinfo, FreeInfoFunctor> address_list;

	socklen_t m_sockaddrlen;
	std::unique_ptr<char[]> m_sockaddr;
	std::string m_address;
	std::string m_service;

 public:
	address(const std::string & address, const std::string & service);

	address(const sockaddr * addr, socklen_t addrlen);

	address(const address & other);

	~address();

	void resolve_sock_addr(int flags = (NI_NUMERICSERV));

	const sockaddr * addr() const
	{
		if (!m_sockaddr)
		{
			
		}
		return reinterpret_cast<struct sockaddr*>(m_sockaddr.get());
	}

	socklen_t addrlen() const
	{ return m_sockaddrlen; }

	void print();

	static std::vector<address>
	list_passive_addresses(const int type, const std::string & portNumber);
};

} // namespace net

#endif // GUARD_ADDRESS_H
