#ifndef GUARD_TCP_WORK_SOCKET_H
#define GUARD_TCP_WORK_SOCKET_H 1

#include "tcp_server_socket.h"

namespace net {

// XXX rename me
class tcp_work_socket : public socket
{
 public:
	virtual ~tcp_work_socket()
		{ }

	virtual void do_read() = 0;

	virtual void do_write() = 0;

 protected:
	tcp_work_socket(int _fd, int _flags, socket_option_list & list)
	  : socket(_fd, _flags, list)
		{ }
};

} // namespace net

#endif // GUARD_TCP_WORK_SOCKET_H
