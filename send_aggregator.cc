#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include <utility>
#include <memory>
#include <chrono>

#include "utility/util.h"
#include "address.h"

// This is a basic little stub of code prototyping how to aggregate socket
// sends into a single syscall - untested but checked in here so as not to
// loose it... sendmmsg() is a syscall that only appeared in the Linux 3.0+
// Kernel and not usable on RH 6

class descriptor {
 public:
	descriptor(int _fd = -1) : fd(_fd) { }

	descriptor(const descriptor & d) = delete;

	descriptor & operator = (const descriptor & d) = delete;

	descriptor(descriptor && other) : fd(other.fd) { other.fd = -1; }

	descriptor & operator = (descriptor && other)
		{ std::swap(fd, other.fd); return *this; }

	descriptor & operator = (int _fd)
		{ fd = _fd; return *this; }

	~descriptor() { if (fd >= 0) close(fd); }

	operator int() const { return fd; }

 protected:
	int fd;
};

std::chrono::nanoseconds
packet_delay(uint64_t bit_rate, uint64_t payload_length)
{
	static const uint64_t one_billion = 1000000000;

	printf("old = %lu, new = %lu\n",
	       (one_billion / (bit_rate / 8 / payload_length)),
		   ((8 * one_billion) * payload_length) / bit_rate);
	return std::chrono::nanoseconds(
		((8 * one_billion) * payload_length) / bit_rate);
}

struct send_aggregator
{
	send_aggregator(const descriptor & d, unsigned int max_msgs = 2)
	  : sfd(&d)
	  , iovs(new iovec[max_msgs])
	  , hdr_defs(new mmsghdr[max_msgs])
	  , max(max_msgs)
	  , count(0)
	{
		memset(iovs.get(), 0, sizeof(iovec) * max_msgs);
		memset(hdr_defs.get(), 0, sizeof(mmsghdr) * max_msgs);
	}

	void queue_message(const net::address & a, const char * buffer, size_t len)
	{
		struct mmsghdr * current = &(hdr_defs[count]);

		iovs[count].iov_base = const_cast<char*>(buffer);
		iovs[count].iov_len = len;

		current->msg_hdr.msg_name = const_cast<sockaddr*>(a.get_addr());
		current->msg_hdr.msg_namelen = a.get_addrlen();
		current->msg_hdr.msg_iov = &(iovs[count]);
		current->msg_hdr.msg_iovlen = 1;
		current->msg_hdr.msg_control = nullptr;
		current->msg_hdr.msg_controllen = 0;
		current->msg_hdr.msg_flags = 0;

		++count;

		if (count >= max)
			throw std::logic_error("OOPS");

	}

	void send_queued()
	{
		if (sendmmsg(*sfd, hdr_defs.get(), count, 0) != count)
		{
			throw make_syserr("sendmmsg failed");
		}
	}

	const descriptor * sfd;
	std::unique_ptr<struct iovec[]> iovs;
	std::unique_ptr<struct mmsghdr[]> hdr_defs;
	int max;
	int count;
};
