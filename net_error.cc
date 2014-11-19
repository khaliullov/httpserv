#include "net_error.h"

#include <netdb.h>

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

} // namespace net
