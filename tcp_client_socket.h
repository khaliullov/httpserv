#ifndef GUARD_TCP_WORK_SOCKET_H
#define GUARD_TCP_WORK_SOCKET_H 1

#include <unistd.h>

#include "tcp_server_socket.h"

namespace net {

class tcp_client_socket : public socket
{
 public:
	// XXX add flags for arg 3
	tcp_client_socket(const address & a)
	  : socket(a.get_addr()->sa_family, SOCK_STREAM, 0)
	  , remote_addr(a)
	{
		if (connect(fd, a.get_addr(), a.get_addrlen()) < 0)
			throw make_syserr("Could not connect()");
	}

	virtual ~tcp_client_socket()
		{ }

	int send_data(const char * buf, size_t len)
	{
		return write(fd, buf, len);
	}

//	virtual void do_read() = 0;

//	virtual void do_write() = 0;

 protected:
	address remote_addr;
};

} // namespace net

#endif // GUARD_TCP_WORK_SOCKET_H
