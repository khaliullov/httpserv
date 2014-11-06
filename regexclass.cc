#include "regexclass.h"
#include <cstring>

namespace util {
regex::regex(const char * r, int _flags)
  : raw(r)
  , compiled(new regex_t)
  , flags(_flags)
{
	memset(compiled.get(), 0, sizeof(regex_t));

	int rc;

	if ((rc = regcomp(compiled.get(), raw.c_str(), flags)) != 0)
	{
		char buffer[80];
		regerror(rc, compiled.get(), buffer, 80);
		throw std::runtime_error(std::string("Bad regex: ") + buffer);
	}
}

regex::regex(const regex & other)
  : regex(other.raw.c_str(), other.flags)
	{ }

regex::regex(regex && r)
  : raw(std::move(r.raw))
  , compiled(std::move(r.compiled))
	{ }

regex::~regex()
{
	if (compiled)
		regfree(compiled.get());
}

bool
regex::match(const string_type & s, size_type & beg, size_type & end) const
{
	bool rc = false;
	regmatch_t match = { 0, 0 };

	if (regexec(compiled.get(), s.c_str(), 1, &match, 0) == 0)
	{
		rc = true;
		beg = match.rm_so;
		end = match.rm_eo;
	}
	return rc;
}

std::vector<regmatch_t>
regex::match(const string_type & s, size_t max_matches) const
{
	std::vector<regmatch_t> ret(max_matches, regmatch_t{-1, -1});

	if (regexec(compiled.get(), s.c_str(), max_matches, ret.data(), 0) != 0)
		ret.clear();

	return ret;
}

} // namespace util
