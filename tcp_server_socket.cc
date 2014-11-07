#include <unistd.h>

#include "tcp_server_socket.h"

#include "utility/util.h"

namespace net {

//////////////////////////////////////////////////////////////////////
// socket_option
//
socket_option::socket_option(int _level, int _optname, int val)
  : level(_level)
  , optname(_optname)
  , length(sizeof(int))
  , option_content(new char[sizeof(int)])
{
	memcpy(option_content.get(), &val, length);
}

socket_option::socket_option(int _level, int _optname,
                             void * _optval, socklen_t _len)
  : level(_level)
  , optname(_optname)
  , length(_len)
  , option_content(new char[length])
{
	memcpy(option_content.get(), _optval, length);
}

socket_option::socket_option(const socket_option & other)
  : level(other.level)
  , optname(other.optname)
  , length(other.length)
  , option_content(new char[length])
{
	memcpy(option_content.get(), other.option_content.get(), length);
}

socket_option::~socket_option()
	{ }

void socket_option::set(int fd) const
{
	if (setsockopt(fd, level, optname, option_content.get(), length) != 0)
		throw make_syserr("Could not set socket option");
}

//////////////////////////////////////////////////////////////////////
// socket
//
socket::socket(int domain, int type, int protocol)
  : fd(::socket(domain, type, protocol))
{
	if (fd < 0) throw std::runtime_error("Could not create socket");
}

socket::socket(int domain, int type, int protocol, const socket_option_list & list)
  : socket(domain, type, protocol)
{
	set_options(list);
}

socket::socket(socket && other) noexcept
  : fd(other.fd)
{
	other.fd = -1;
}

socket & socket::operator = (socket && other) noexcept
{
	std::swap(fd, other.fd);
	return *this;
}

void socket::set_options(const socket_option_list & list)
{
	for (const auto & o : list)
		o.set(fd);
}

socket::~socket()
	{ if (fd > 0) ::close(fd); }

//////////////////////////////////////////////////////////////////////
// tcp_server_socket
//
tcp_server_socket::tcp_server_socket(const address & a)
  : tcp_server_socket(a, socket_option_list{})
	{ }

tcp_server_socket::tcp_server_socket(const address & a,
                                     const socket_option_list & opts)
  : socket(a.get_addr()->sa_family, SOCK_STREAM, 0, opts)
  , m_address(a)
{
	if (bind(fd, m_address.get_addr(), m_address.get_addrlen()) != 0)
		throw make_syserr("Could not bind to socket");

	if (listen(fd, 5) != 0)
		throw make_syserr("Could not bind to listen");
}

tcp_server_socket::tcp_server_socket(tcp_server_socket && other) noexcept
  : socket(std::move(other))
  , m_address(std::move(other.m_address))
	{ }

tcp_server_socket &
tcp_server_socket::operator = (tcp_server_socket && other) noexcept
{
	using std::swap;
	net::socket::operator=(std::move(other));
	swap(m_address, other.m_address);
	return *this;
}

} // namespace net
