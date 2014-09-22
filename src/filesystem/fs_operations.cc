#include "fs_operations.h"
#include "filesystem_error.h"

#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

#include <cstdio>  // _P_tmpdir is defined here
#include <climits> // PATH_MAX is defined through this

namespace filesystem {
inline namespace v1 {

inline constexpr file_type st_mode_to_file_type(mode_t m)
{
	return ( (S_ISREG(m) ? file_type::regular : 
	         (S_ISDIR(m) ? file_type::directory :
	         (S_ISCHR(m) ? file_type::character :
	         (S_ISBLK(m) ? file_type::block :
	         (S_ISFIFO(m) ? file_type::fifo :
	         (S_ISLNK(m) ? file_type::symlink :
	         (S_ISSOCK(m) ? file_type::socket :
	         (file_type::unknown) ) ) ) ) ) ) ) );
}

inline constexpr perms st_mode_to_perms(mode_t m)
{
	return ( ( (m & S_IRUSR) ? perms::owner_read : perms::none )
	       | ( (m & S_IWUSR) ? perms::owner_write : perms::none )
	       | ( (m & S_IXUSR) ? perms::owner_exec : perms::none )
	       | ( (m & S_IRGRP) ? perms::group_read : perms::none )
	       | ( (m & S_IWGRP) ? perms::group_write : perms::none )
	       | ( (m & S_IXGRP) ? perms::group_exec : perms::none )
	       | ( (m & S_IROTH) ? perms::others_read : perms::none )
	       | ( (m & S_IWOTH) ? perms::others_write : perms::none )
	       | ( (m & S_IXOTH) ? perms::others_exec : perms::none )
	       | ( (m & S_ISUID) ? perms::set_uid : perms::none )
	       | ( (m & S_ISGID) ? perms::set_gid : perms::none )
	       | ( (m & S_ISVTX) ? perms::sticky_bit : perms::none ) );
}

path current_path()
{
	std::error_code ec;
	path retval = current_path(ec);
	if (ec)
		throw filesystem_error("Could not read current working directory", ec);
	return retval;
}

path current_path(std::error_code & ec)
{
	path retval;
	char cwd_buffer[PATH_MAX];
	ec.clear();

	if (getcwd(cwd_buffer, PATH_MAX) == nullptr)
		ec = make_errno_ec();
	else
		retval = cwd_buffer;

	return retval;
}

void current_path(const path & p)
{
	std::error_code ec;
	current_path(p, ec);
	if (ec)
		throw filesystem_error("Could not change working directory", p, ec);
}

void current_path(const path & p, std::error_code & ec) noexcept
{
	ec.clear();

	if (chdir(p.c_str()) != 0)
		ec = make_errno_ec();
}

uintmax_t file_size(const path & p)
{
	std::error_code  ec;
	uintmax_t ret = file_size(p, ec);
	if (ec) throw filesystem_error("Could not read file size", p, ec);
	return ret;
}

uintmax_t file_size(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	uintmax_t ret = static_cast<uintmax_t>(-1);

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
	{
		if (S_ISREG(st.st_mode))
			ret = st.st_size;
		else
			ec = std::make_error_code(std::errc::not_supported);
	} else
		ec = make_errno_ec();

	return ret;
}

uintmax_t hard_link_count(const path & p)
{
	std::error_code  ec;
	uintmax_t ret = hard_link_count(p, ec);
	if (ec) throw filesystem_error("Could not read link count", p, ec);
	return ret;
}

uintmax_t hard_link_count(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	uintmax_t ret = static_cast<uintmax_t>(-1);

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
		ret = st.st_nlink;
	else
		ec = make_errno_ec();

	return ret;
}

bool is_block_file(const path & p)
	{ return is_block_file(status(p)); }

bool is_block_file(const path & p, std::error_code & ec) noexcept
	{ return is_block_file(status(p, ec)); }

bool is_character_file(const path & p)
	{ return is_character_file(status(p)); }

bool is_character_file(const path & p, std::error_code & ec) noexcept
	{ return is_character_file(status(p, ec)); }

bool is_directory(const path & p)
	{ return is_directory(status(p)); }

bool is_directory(const path & p, std::error_code & ec) noexcept
	{ return is_directory(status(p, ec)); }

bool is_fifo(const path & p)
	{ return is_fifo(status(p)); }

bool is_fifo(const path & p, std::error_code & ec) noexcept
	{ return is_fifo(status(p, ec)); }

bool is_other(const path & p)
	{ return is_other(symlink_status(p)); }

bool is_other(const path & p, std::error_code & ec) noexcept
	{ return is_other(symlink_status(p, ec)); }

bool is_regular_file(const path & p)
	{ return is_regular_file(status(p)); }

bool is_regular_file(const path & p, std::error_code & ec) noexcept
	{ return is_regular_file(status(p, ec)); }

bool is_socket(const path & p)
	{ return is_socket(status(p)); }

bool is_socket(const path & p, std::error_code & ec) noexcept
	{ return is_socket(status(p, ec)); }

bool is_symlink(const path & p)
	{ return is_symlink(symlink_status(p)); }

bool is_symlink(const path & p, std::error_code & ec) noexcept
	{ return is_symlink(symlink_status(p, ec)); }

space_info space(const path & p)
{
	std::error_code ec;
	space_info info = space(p, ec);
	if (ec) throw filesystem_error("Could not determine size information "
	                               "for filesystem", p, ec);
	return info;
}

space_info space(const path & p, std::error_code & ec) noexcept
{
	struct statvfs stvfs;
	space_info info = { static_cast<uintmax_t>(-1),
	                    static_cast<uintmax_t>(-1),
	                    static_cast<uintmax_t>(-1) };


	memset(&stvfs, 0, sizeof(stvfs));

	ec.clear();

	if (statvfs(p.c_str(), &stvfs) == 0)
	{
		info.capacity = stvfs.f_blocks;
		info.free = stvfs.f_bfree;
		info.available = stvfs.f_bavail;

		info.capacity *= stvfs.f_frsize;
		info.free *= stvfs.f_frsize;
		info.available *= stvfs.f_frsize;
	} else
		ec = make_errno_ec();

	return info;
}

file_status status(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	file_status ret;

	ec.clear();

	if (stat(p.c_str(), &st) == 0)
	{
		ret.type(st_mode_to_file_type(st.st_mode));
		ret.permissions(st_mode_to_perms(st.st_mode));
	} else if (errno == ENOENT)
		ret.type(file_type::not_found);
	else
		ec = make_errno_ec();

	return ret;
}

file_status status(const path & p)
{
	std::error_code  ec;
	file_status ret = status(p, ec);
	if (ec) throw filesystem_error("Could not stat file", p, ec);
	return ret;
}

file_status symlink_status(const path & p)
{
	std::error_code  ec;
	file_status ret = symlink_status(p, ec);
	if (ec) throw filesystem_error("Could not stat file", p, ec);
	return ret;
}

file_status symlink_status(const path & p, std::error_code & ec) noexcept
{
	struct stat st;
	file_status ret;

	ec.clear();

	if (lstat(p.c_str(), &st) == 0)
	{
		ret.type(st_mode_to_file_type(st.st_mode));
		ret.permissions(st_mode_to_perms(st.st_mode));
	} else if (errno == ENOENT)
		ret.type(file_type::not_found);
	else
		ec = make_errno_ec();

	return ret;
}

path temp_directory_path()
{
	std::error_code  ec;
	path ret = temp_directory_path(ec);
	if (ec) throw filesystem_error("No suitable temp directory found", ec);
	return ret;
}

path temp_directory_path(std::error_code & ec)
{
	path tmpdir{P_tmpdir};
	file_status st;

	for (const auto var : { "TMPDIR", "TMP", "TEMP", "TEMPDIR" })
	{
		const char * value = getenv(var);
		if (value != nullptr)
		{
			printf("Found var %s = %s\n", var, value);
			st = status(value, ec);

			if (exists(st) && is_directory(st))
			{
				tmpdir = value;
				break;
			}
		}
	}

	ec.clear();
	st = status(tmpdir, ec);
	if (!exists(st) || !is_directory(st))
		tmpdir.clear();

	return tmpdir;
}

}
}
