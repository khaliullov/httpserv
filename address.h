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
 public:
	address(const std::string & address, const std::string & service,
                 const sockaddr * a, socklen_t length);

	address(const sockaddr * addr, socklen_t addrlen);

	address(const address & other);

	address(address && other) noexcept;

	~address();
	
	address & operator = (const address & other);

	address & operator = (address && other) noexcept;

	void resolve_binary_addr(int flags = (NI_NUMERICSERV | NI_NUMERICHOST));

	const sockaddr * get_addr() const
	{ return reinterpret_cast<struct sockaddr*>(m_sockaddr.get()); }

	socklen_t get_addrlen() const
	{ return m_sockaddrlen; }

	std::string str() const;

	static std::vector<address> get_addresses(const int type,
	                                          const std::string & node,
	                                          const std::string & portNumber);

	static std::vector<address> passive_addresses(const int type,
	                                              const std::string & port);

 protected:
	struct free_addrinfo_f {
		void operator () (struct addrinfo * p) { freeaddrinfo(p); }
	};

	typedef std::unique_ptr<struct addrinfo, free_addrinfo_f> address_list;

	socklen_t m_sockaddrlen;
	std::unique_ptr<char[]> m_sockaddr;
	std::string m_address;
	std::string m_service;

};

} // namespace net

#endif // GUARD_ADDRESS_H
