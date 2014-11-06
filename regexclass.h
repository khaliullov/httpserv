#ifndef GUARD_REGEX_CLASS_H
#define GUARD_REGEX_CLASS_H 1

#include <regex.h>

#include <vector>
#include <string>
#include <memory>

namespace util {

class regex
{
 public:
	typedef std::string                     string_type;
	typedef typename std::string::size_type size_type;

	regex(const char * r, int _flags = REG_EXTENDED);

	regex(const regex & other);

	regex(regex && r);

	~regex();

	bool match(const string_type & s, size_type & beg, size_type & end) const;

	std::vector<regmatch_t>
	match(const string_type & s, size_t max_matches = 32) const;

 protected:
	string_type raw;
	std::unique_ptr<regex_t> compiled;
	int flags;
};

} // namespace util
#endif // GUARD_REGEX_CLASS_H

