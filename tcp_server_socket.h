#ifndef GUARD_TCP_SERVER_SOCKET
#define GUARD_TCP_SERVER_SOCKET 1

#include <sys/types.h>
#include <sys/socket.h>
#include <tuple>
#include <vector>
#include <memory>

#include "utility/util.h"
#include "address.h"

namespace net {

//////////////////////////////////////////////////////////////////////
struct socket_option
{
 public:
	socket_option(int _level, int _optname, int val);

	socket_option(int _level, int _optname, void * _optval, socklen_t _len);

	socket_option(const socket_option & other);

	~socket_option();

	void set(int fd) const;

 private:
	int level;
	int optname;
	socklen_t length;
	std::unique_ptr<char[]> option_content;
};

//////////////////////////////////////////////////////////////////////
typedef std::vector<socket_option> socket_option_list;

//////////////////////////////////////////////////////////////////////
class socket
{
 public:
	static const int default_flags = (SOCK_CLOEXEC | SOCK_NONBLOCK);

	socket(int domain, int type, int protocol = 0);

	socket(int domain, int type, int protocol, const socket_option_list & list);

	socket(const socket & other)  = delete;

	socket & operator = (const socket & other)  = delete;

	socket(socket && other) noexcept;

	socket & operator = (socket && other) noexcept;

	void set_options(const socket_option_list & list);

	virtual ~socket();

	operator int () const { return fd; }

	bool is_nonblocking();

	bool will_close_on_exec();

 protected:
	socket(int fd, int flags, const socket_option_list & list);

	int fd;
	int flags;
};

//////////////////////////////////////////////////////////////////////
class tcp_listening_socket : public net::socket
{
 public:

	tcp_listening_socket(const address & a, int flags = default_flags);

	tcp_listening_socket(const address & a,
	                  const socket_option_list & opts,
	                  int flags = default_flags);

	tcp_listening_socket(const tcp_listening_socket &) = delete;

	tcp_listening_socket & operator = (const tcp_listening_socket &) = delete;

	tcp_listening_socket(tcp_listening_socket && other) noexcept;

	tcp_listening_socket & operator = (tcp_listening_socket && other) noexcept;

	std::tuple<int, address> accept(int flags = default_flags);

	template<typename T>
	std::unique_ptr<T> accept_alt(int flags = default_flags)
	{
		struct sockaddr_storage buf;
		socklen_t len = sizeof(sockaddr_storage);
		std::unique_ptr<T> return_ptr;

		int sfd = accept4(fd, reinterpret_cast<sockaddr*>(&buf), &len, flags);

		if (sfd < 0)
		{
			if ( ! (this->is_nonblocking() && errno == EAGAIN) )
				throw make_syserr("Could not accept connection");
		} else
		{
			auto addr = address{reinterpret_cast<sockaddr*>(&buf), len};
			return_ptr.reset(new T(sfd, addr));
		}
		return return_ptr;
	}

 protected:
	address m_address;
};

//////////////////////////////////////////////////////////////////////
class tcp_server_socket : public net::socket
{
 public:
	tcp_server_socket(int fd, const address & a, int flags = default_flags)
	  : socket(fd, flags, socket_option_list())
	  , m_address(a)
		{ }

	tcp_server_socket(int fd, const address & a, int flags,
	                  const socket_option_list & opts)
	  : socket(fd, flags, opts)
	  , m_address(a)
		{ }

	tcp_server_socket(const tcp_server_socket &) = delete;

	tcp_server_socket & operator = (const tcp_server_socket &) = delete;

	tcp_server_socket(tcp_server_socket && other) noexcept
	  : socket(std::move(other))
	  , m_address(std::move(other.m_address))
		{ }

	tcp_server_socket & operator = (tcp_server_socket && other) noexcept
	{
		using std::swap;
		net::socket::operator=(std::move(other));
		swap(m_address, other.m_address);
		return * this;
	}

	virtual ~tcp_server_socket()
	{
//		if (fd >= 0)
//			printf("Destructor - %s\n", __PRETTY_FUNCTION__);
	}

	const address & client_address() const
		{ return m_address; }

 protected:
	address m_address;
};

} // namespace net

#endif // GUARD_TCP_SERVER_SOCKET
