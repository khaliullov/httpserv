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

#include "utility/util.h"
#include "address.h"
#include "net_error.h"

namespace net {

//////////////////////////////////////////////////////////////////////
struct socket_option
{
 public:
	socket_option(int _level, int _optname, void * _optval, socklen_t _len)
	  : level(_level)
	  , optname(_optname)
	  , length(_len)
	  , option_content(new char[length])
	{
		memcpy(option_content.get(), _optval, length);
	}

	~socket_option() { }

	void set(int fd) const
	{
		if (setsockopt(fd, level, optname, option_content.get(), length) != 0)
			throw make_syserr("Could not set socket option");
	}

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
	socket(int domain, int type, int protocol = 0)
	  : fd(::socket(domain, type, protocol))
		{ if (fd < 0) throw std::runtime_error("Could not create socket"); }

	void set_options(const socket_option_list & list)
		{ for (const auto & o : list) o.set(fd); }

	virtual ~socket() { close(fd); }

 protected:
	int fd;
};

//////////////////////////////////////////////////////////////////////
class tcp_server_socket : net::socket {
 public:
	tcp_server_socket(const address & a)
	  : socket(a.get_addr()->sa_family, SOCK_STREAM)
	  , m_address(a)
	{
	}

	tcp_server_socket(const address & a, const socket_option_list & opts)
	  : tcp_server_socket(a)
	{
		this->set_options(opts);
	}

 private:
	address m_address;
};

} // namespace net
