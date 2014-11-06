#ifndef GUARD_NET_ERROR_H
#define GUARD_NET_ERROR_H 1

#include <netdb.h>

#include <system_error>

namespace net {

//////////////////////////////////////////////////////////////////////
class ainfo_error_category : public std::error_category
{
 public:
	ainfo_error_category() noexcept : std::error_category() { }

	~ainfo_error_category() { };

	const char * name() const noexcept override
	{ return "addrinfo"; }

	std::string message(int cond) const override
	{ return gai_strerror(cond); }

	ainfo_error_category(const ainfo_error_category &) = delete;

	ainfo_error_category &
	operator = (const ainfo_error_category&) = delete;
};

//////////////////////////////////////////////////////////////////////
const std::error_category & addrinfo_category()
{
	static ainfo_error_category the_category;
	return the_category;
}

//////////////////////////////////////////////////////////////////////
std::system_error make_ainfo_error(int e, const char * what)
	{ return std::system_error(e, net::addrinfo_category(), what); }

std::system_error make_ainfo_error(int e, const std::string & what)
	{ return std::system_error(e, net::addrinfo_category(), what); }

std::system_error make_ainfo_error(int e)
	{ return std::system_error(e, net::addrinfo_category()); }

} // namespace net

#endif // GUARD_NET_ERROR_H
