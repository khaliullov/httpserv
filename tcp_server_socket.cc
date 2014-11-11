#include <sys/types.h>
#include <sys/socket.h>
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
  , flags(type & (SOCK_NONBLOCK | SOCK_CLOEXEC))
{
	if (fd < 0) throw std::runtime_error("Could not create socket");
}

// For creating sockets from accept()
socket::socket(int _fd, int _flags, const socket_option_list & list)
  : fd(_fd)
  , flags(_flags)
{
	set_options(list);
}

socket::socket(int domain, int type, int protocol,
               const socket_option_list & list)
  : socket(domain, type, protocol)
{
	set_options(list);
}

socket::socket(socket && other) noexcept
  : fd(other.fd)
  , flags(other.flags)
{
	other.fd = -1;
}

socket & socket::operator = (socket && other) noexcept
{
	std::swap(fd, other.fd);
	std::swap(flags, other.flags);
	return *this;
}

void socket::set_options(const socket_option_list & list)
{
	for (const auto & o : list)
		o.set(fd);
}

socket::~socket()
	{ if (fd > 0) ::close(fd); }

bool socket::is_nonblocking()
	{ return (flags & SOCK_NONBLOCK); }

bool socket::will_close_on_exec()
	{ return (flags & SOCK_CLOEXEC); }

//////////////////////////////////////////////////////////////////////
// tcp_server_socket
//
tcp_server_socket::tcp_server_socket(const address & a, int flags)
  : tcp_server_socket(a, socket_option_list{}, flags)
	{ }

tcp_server_socket::tcp_server_socket(const address & a,
                                     const socket_option_list & opts,
                                     int flags)
  : socket(a.get_addr()->sa_family, SOCK_STREAM | flags, 0, opts)
  , m_address(a)
{
	if (bind(fd, m_address.get_addr(), m_address.get_addrlen()) != 0)
		throw make_syserr("Could not bind to socket");

	if (listen(fd, 128) != 0)
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

std::tuple<int, address> tcp_server_socket::accept(int flags)
{
	struct sockaddr_storage buf;
	socklen_t len = sizeof(sockaddr_storage);

	std::tuple<int, address> ret;

	
	std::get<0>(ret) = accept4(fd, reinterpret_cast<sockaddr*>(&buf),
	                           &len, flags);

	if (std::get<0>(ret) < 0)
	{
		if ( ! (this->is_nonblocking() && errno == EAGAIN) )
			throw make_syserr("Could not accept connection");
	} else
	{
		std::get<1>(ret) = address{reinterpret_cast<sockaddr*>(&buf), len};
	}

	return ret;
}

} // namespace net
