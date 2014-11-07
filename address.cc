#include "address.h"

namespace net {

//////////////////////////////////////////////////////////////////////
address::address(const std::string & address, const std::string & service,
                 const sockaddr * a, socklen_t length)
  : m_sockaddrlen(length)
  , m_sockaddr(new char[length])
  , m_address(address)
  , m_service(service)
{
	memcpy(m_sockaddr.get(), a, m_sockaddrlen);
}

//////////////////////////////////////////////////////////////////////
address::address(const sockaddr * addr, socklen_t addrlen)
  : m_sockaddrlen(addrlen)
  , m_sockaddr(m_sockaddrlen ? new char[m_sockaddrlen] : nullptr)
  , m_address()
  , m_service()
{
	memcpy(m_sockaddr.get(), addr, addrlen);
	resolve_binary_addr();
}

//////////////////////////////////////////////////////////////////////
address::address(const address & other)
  : m_sockaddrlen(other.m_sockaddrlen)
  , m_sockaddr(m_sockaddrlen ? new char[m_sockaddrlen] : nullptr)
  , m_address(other.m_address)
  , m_service(other.m_service)
{
	memcpy(m_sockaddr.get(), other.m_sockaddr.get(), m_sockaddrlen);
}

//////////////////////////////////////////////////////////////////////
address::address(address && other) noexcept
  : m_sockaddrlen(other.m_sockaddrlen)
  , m_sockaddr(std::move(other.m_sockaddr))
  , m_address(std::move(other.m_address))
  , m_service(std::move(other.m_service))
{
}

//////////////////////////////////////////////////////////////////////
address::~address()
	{ }

//////////////////////////////////////////////////////////////////////
address & address::operator = (const address & other)
{
	if (this != &other)
	{
		m_sockaddrlen = other.m_sockaddrlen;
		if (m_sockaddrlen > 0)
		{
			m_sockaddr.reset(new char[m_sockaddrlen]);
			memcpy(m_sockaddr.get(), other.m_sockaddr.get(), m_sockaddrlen);
		}
		m_address = other.m_address;
		m_service = other.m_service;
	}

	return *this;
}

//////////////////////////////////////////////////////////////////////
address & address::operator = (address && other) noexcept
{
	using std::swap;

	swap(m_sockaddrlen, other.m_sockaddrlen);
	swap(m_sockaddr, other.m_sockaddr);
	swap(m_address, other.m_address);
	swap(m_service, other.m_service);

	return *this;
}

//////////////////////////////////////////////////////////////////////
void address::resolve_binary_addr(int flags)
{
	const int buflen = 80;
	char buffer1[buflen], buffer2[buflen];

	int rc = getnameinfo(reinterpret_cast<struct sockaddr*>(m_sockaddr.get()),
	                     m_sockaddrlen, buffer1, buflen,
	                     buffer2, buflen, flags);

	if (rc) throw net::make_ainfo_error(rc, __func__);
	m_address = buffer1;
	m_service = buffer2;
}

//////////////////////////////////////////////////////////////////////
std::string address::str() const
{
	std::string ret;
	if (m_address.find(":") != std::string::npos)
	{
		ret = '[' + m_address + "]:" + m_service;
	} else
	{
		ret = m_address + ':' + m_service;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////
std::vector<address>
address::get_addresses(const int type,
                       const std::string & node,
                       const std::string & portNumber)
{
	std::vector<address> retlist;
	int rc = 0;
	struct addrinfo * tmp = nullptr;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;
	hints.ai_flags = 0;
	
	if ((rc = getaddrinfo(node.c_str(), portNumber.c_str(), &hints, &tmp)) != 0)
		throw net::make_ainfo_error(rc, __func__);

	address_list info(tmp);

	for (tmp = info.get(); tmp != nullptr; tmp = tmp->ai_next)
		retlist.emplace_back(tmp->ai_addr, tmp->ai_addrlen);

	return retlist;
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
		retlist.emplace_back(tmp->ai_addr, tmp->ai_addrlen);

	return retlist;
}

} // namespace net
