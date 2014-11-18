#ifndef GUARD_NET_ERROR_H
#define GUARD_NET_ERROR_H 1

#include <system_error>

namespace net {

//////////////////////////////////////////////////////////////////////
const std::error_category & addrinfo_category();

//////////////////////////////////////////////////////////////////////
inline std::system_error make_ainfo_error(int e, const char * what)
	{ return std::system_error(e, net::addrinfo_category(), what); }

inline std::system_error make_ainfo_error(int e, const std::string & what)
	{ return std::system_error(e, net::addrinfo_category(), what); }

inline std::system_error make_ainfo_error(int e)
	{ return std::system_error(e, net::addrinfo_category()); }

} // namespace net

#endif // GUARD_NET_ERROR_H
