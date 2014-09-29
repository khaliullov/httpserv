#include "recursive_directory_iterator.h"
#include "directory_iterator.h"
#include "path.h"

namespace filesystem {
inline namespace v1 {

// Constructs the end() iterator
recursive_directory_iterator::recursive_directory_iterator() noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(directory_options::none)
  , m_pathname()
  , m_current_path()
  , m_depth(0)
  , m_entry()
{
	// not really necessary here
	set_to_end_iterator();
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options)
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_current_path(p)
  , m_depth(0)
  , m_entry()
{
	std::error_code ec;
	delegate_construction(ec);
	if (ec)
		throw filesystem_error("Could not read directory entry", p, ec);
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, directory_options options, std::error_code & ec) noexcept
  : m_handle(nullptr)
  , m_buffer()
  , m_options(options)
  , m_pathname(p)
  , m_current_path(p)
  , m_depth(0)
  , m_entry()
{
	delegate_construction(ec);
}

recursive_directory_iterator::recursive_directory_iterator(
	const path & p, std::error_code & ec) noexcept
  : recursive_directory_iterator(p, directory_options::none, ec)
	{ }

recursive_directory_iterator::recursive_directory_iterator(const path & p)
  : recursive_directory_iterator(p, directory_options::none)
	{ }

recursive_directory_iterator::recursive_directory_iterator(
	const recursive_directory_iterator & other)
  : recursive_directory_iterator(other.m_pathname, other.m_options)
	{ }

recursive_directory_iterator::recursive_directory_iterator(
	recursive_directory_iterator && ) noexcept
{
	// FIXME
}

recursive_directory_iterator::~recursive_directory_iterator()
	{ }


void recursive_directory_iterator::delegate_construction(std::error_code & ec)
{
	ec.clear();

	if (m_pathname.empty())
	{
		ec = std::make_error_code(std::errc::invalid_argument);
		m_pathname.clear();
		return;
	}

	m_handle.reset(opendir(m_pathname.c_str()));

	if (! m_handle)
	{
		ec = make_errno_ec();
		m_handle.reset();
		m_pathname.clear();
		return;
	}

	m_pathname /= "/";
	m_entry.assign(m_pathname);
	printf("----------------------> %s\n", m_pathname.c_str());
	increment(ec);
}

std::error_code recursive_directory_iterator::push_state()
{
	std::error_code ec;
	long location = telldir(m_handle.get());

	if (location == -1)
		ec = make_errno_ec();
	else
		m_stack.emplace(m_current_path, telldir(m_handle.get()));
	return ec;
}

void recursive_directory_iterator::pop()
{
	m_handle.reset(opendir(m_stack.top().directory.c_str()));
	if (! m_handle)
		throw filesystem_error("Could not re-open directory",
		                       m_stack.top().directory.c_str(),
		                       make_errno_ec());

	seekdir(m_handle.get(), m_stack.top().tellptr);
	m_current_path = m_stack.top().directory;
	m_entry.assign(m_current_path / "/");
	m_stack.pop();
}

std::error_code recursive_directory_iterator::do_recursive_open(const path & p)
{
	std::error_code ec;

	push_state();

	printf("Opening directory '%s'\n", p.c_str());
	m_current_path = p;
	m_handle.reset(opendir(m_current_path.c_str()));

	if (! m_handle)
	{
		printf("-------> OPEN FAILED!!!!: %s\n", strerror(errno));
		ec = make_errno_ec();
		m_handle.reset();
		m_pathname.clear();
	} else
	{
		printf("-------> OPEN SUCCEEDED!!!!: %s\n", m_current_path.c_str());
		m_current_path /= "/";
		m_entry.assign(m_current_path);
	}

	return ec;
}

bool recursive_directory_iterator::recursion_pending() const
{
	bool rc = false;

//	if ( ! (  m_entry.path().empty()
//	       || is_linking_directory(m_entry) ) )

	if (m_entry.path().empty())
	{
//		printf("%s is empty\n", m_entry.path().c_str());
	} else if (is_linking_directory(m_entry))
	{
//		printf("%s is linking dir\n", m_entry.path().c_str());
	} else
	{
		rc = (m_entry.symlink_status().type() == file_type::directory);

		if (rc) printf("Recursing on '%s'\n", m_entry.path().c_str());
	}
	return rc;
}


recursive_directory_iterator &
recursive_directory_iterator::increment(std::error_code & ec) noexcept
{
	struct dirent * de;
	int rv = 0;
	ec.clear();

	// Bail on invalid
	if (! m_handle)
	{
		set_to_end_iterator();
		ec = std::make_error_code(std::errc::bad_file_descriptor);
		return *this;
	}

	if (!m_entry.path().empty() && recursion_pending())
	{
		if ((ec = do_recursive_open(m_entry)))
		{
			printf("************Error occurred %d: %s\n", ec.value(),
			       ec.message().c_str());

			// This condition should be:
			// if((ec.default_error_condition() == std::errc::permission_denied)
			// ...but libstdc++ doesn't map system_category errors to
			// generic_category()
			if (  ( (ec.value() == EACCES) || (ec.value() == EPERM) )
			   && ( (m_options & directory_options::skip_permission_denied)
			            != directory_options::none) )
			{
				printf("************* ignoring error\n");
				ec.clear();
				pop();
			} else
			{
				path tmp = m_entry;
				set_to_end_iterator();
				m_current_path = tmp;
				return *this;
			}
		}
	}

	while (  ((rv = readdir_r(m_handle.get(), &m_buffer, &de)) == 0)
	      && (de == nullptr) && !m_stack.empty() )
	{
		pop();
	}

	if (ec) printf("EC IS SET\n");

	if (rv == 0 && de == nullptr && m_stack.empty())
		set_to_end_iterator();
	else if (rv != 0)
		ec = make_errno_ec(rv);
	else
	{
		path tmp(m_current_path);
		tmp /= m_buffer.d_name;
		m_entry.assign(tmp);
	}

	if (ec) printf("EC IS SET2\n");

	return *this;
}

} // inline namespace v1
} // namespace filesystem
