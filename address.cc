
#include "address.h"

namespace net {

//////////////////////////////////////////////////////////////////////
address::address(const std::string & address, const std::string & service)
  : m_sockaddrlen(0)
  , m_sockaddr()
  , m_address(address)
  , m_service(service)
	{ }

//////////////////////////////////////////////////////////////////////
address::address(const sockaddr * addr, socklen_t addrlen)
  : m_sockaddrlen(addrlen)
  , m_sockaddr(m_sockaddrlen ? new char[m_sockaddrlen] : nullptr)
  , m_address()
  , m_service()
	{ memcpy(m_sockaddr.get(), addr, addrlen); }

//////////////////////////////////////////////////////////////////////
address::address(const address & other)
  : m_sockaddrlen(other.m_sockaddrlen)
  , m_sockaddr(m_sockaddrlen ? new char[m_sockaddrlen] : nullptr)
  , m_address(other.m_address)
  , m_service(other.m_service)
	{ memcpy(m_sockaddr.get(), other.m_sockaddr.get(), m_sockaddrlen); }

//////////////////////////////////////////////////////////////////////
address::~address()
	{ }

//////////////////////////////////////////////////////////////////////
void address::resolve_sock_addr(int flags)
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

//////////////////////////////////////////////////////////////////////
void address::print()
{
	if (m_address.empty() || m_service.empty())
		resolve_sock_addr();

	printf("%s:%s\n", m_address.c_str(), m_service.c_str());
}

//////////////////////////////////////////////////////////////////////
std::vector<address>
address::list_passive_addresses(const int type, const std::string & portNumber)
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

} // namespace net
