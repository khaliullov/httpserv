#ifndef GUARD_RECURSIVE_DIRECTORY_ITERATOR_H
#define GUARD_RECURSIVE_DIRECTORY_ITERATOR_H 1

#include <iterator>
#include <queue>
#include "directory_iterator.h"

namespace filesystem {
inline namespace v1 {

class recursive_directory_iterator
  : public std::iterator<std::input_iterator_tag, directory_entry>
{
 public:
	// constructors and destructor
	recursive_directory_iterator() noexcept;

	explicit recursive_directory_iterator(const path & p);

	recursive_directory_iterator(const path & p, directory_options options);

	recursive_directory_iterator(const path & p, directory_options options,
	                             std::error_code & ec) noexcept;

	recursive_directory_iterator(const path & p, std::error_code & ec) noexcept;

	recursive_directory_iterator(const recursive_directory_iterator & rhs);

	recursive_directory_iterator(recursive_directory_iterator && rhs) noexcept;

	~recursive_directory_iterator();

	// modifiers
	recursive_directory_iterator &
	  operator = (const recursive_directory_iterator & rhs);

	recursive_directory_iterator &
	  operator = (recursive_directory_iterator && rhs) noexcept;

	recursive_directory_iterator & operator++()
	{
		std::error_code ec;
		increment(ec);
		if (ec) throw filesystem_error("Could not advance directory cursor",
		                                m_pathname, ec);
		return *this;
	}

	recursive_directory_iterator & increment(std::error_code & ec) noexcept;

	void pop();

	void disable_recursion_pending();
	// observers
	directory_options options() const;

	int depth() const;

	bool recursion_pending() const;

	const directory_entry & operator * () const
		{ return m_entry; }

	const directory_entry * operator -> () const
		{ return &m_entry; }

 private:
	std::unique_ptr<DIR, DirCloseFunctor> m_handle;
	struct dirent                         m_buffer;
	directory_options                     m_options;
	path                                  m_pathname;
	unsigned int                          m_depth;
	directory_entry                       m_entry;
};


inline
recursive_directory_iterator begin(recursive_directory_iterator iter) noexcept
	{ return iter; }

inline
recursive_directory_iterator end(const recursive_directory_iterator &) noexcept
	{ return recursive_directory_iterator(); }

} // inline namespace v1
} // namespace filesystem

#endif // GUARD_RECURSIVE_DIRECTORY_ITERATOR_H
