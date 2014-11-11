#ifndef GUARD_TCP_SERVER_SOCKET
#define GUARD_TCP_SERVER_SOCKET 1

#include <sys/types.h>
#include <sys/socket.h>
#include <tuple>
#include <vector>
#include <memory>

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
class tcp_server_socket : public net::socket
{
 public:
	static const int default_flags = (SOCK_CLOEXEC|SOCK_NONBLOCK);

	tcp_server_socket(const address & a, int flags = default_flags);

	tcp_server_socket(const address & a,
	                  const socket_option_list & opts,
	                  int flags = default_flags);

	tcp_server_socket(const tcp_server_socket &) = delete;

	tcp_server_socket & operator = (const tcp_server_socket &) = delete;

	tcp_server_socket(tcp_server_socket && other) noexcept;

	tcp_server_socket & operator = (tcp_server_socket && other) noexcept;

	std::tuple<int, address> accept(int flags = default_flags);


 protected:
	address m_address;

};

} // namespace net

#endif // GUARD_TCP_SERVER_SOCKET
