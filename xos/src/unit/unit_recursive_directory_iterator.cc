#include "../filesystem/path.h"
#include "../filesystem/file_status.h"
#include "../filesystem/fs_operations.h"
#include "../filesystem/directory_iterator.h"
#include "../filesystem/recursive_directory_iterator.h"

#include <cstdio>
#include <iostream>
#include <set>

#include "cppunit-header.h"

namespace fs = filesystem::v1;

class Test_recursive_directory_iterator : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_recursive_directory_iterator);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST(assignment);
	CPPUNIT_TEST(iteration);
	CPPUNIT_TEST(random_tests);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructors()
	{
		std::error_code ec;
		fs::recursive_directory_iterator empty;

		CPPUNIT_ASSERT(empty == end(fs::recursive_directory_iterator()));

		CPPUNIT_ASSERT_NO_THROW(fs::recursive_directory_iterator i;);
		CPPUNIT_ASSERT_NO_THROW(
			fs::recursive_directory_iterator i(fs::path{"/tmp"}););
		CPPUNIT_ASSERT_NO_THROW(
			fs::recursive_directory_iterator i(fs::path{"/tmp"}, ec););

		CPPUNIT_ASSERT_THROW({
			fs::recursive_directory_iterator j(fs::path("/foo"));
		}, fs::filesystem_error);

		CPPUNIT_ASSERT_THROW({
			fs::recursive_directory_iterator j(fs::path{"/root"});
		}, fs::filesystem_error);

		CPPUNIT_ASSERT_THROW({
			fs::recursive_directory_iterator j(fs::path{""});
		}, fs::filesystem_error);

		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::recursive_directory_iterator j(fs::path("/foo"), ec););
		CPPUNIT_ASSERT(ec);
		               
		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::recursive_directory_iterator j(fs::path("/root"), ec););
		CPPUNIT_ASSERT(ec);
		               
		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::recursive_directory_iterator j(fs::path(""), ec););
		CPPUNIT_ASSERT(ec);

	}

	void assignment()
	{
		fs::recursive_directory_iterator i("/tmp");
		fs::recursive_directory_iterator j("/tmp");
		fs::recursive_directory_iterator empty;

		i = fs::recursive_directory_iterator{};
		i = j;
		i = empty;

		fs::recursive_directory_iterator
		      fromroot("/", fs::directory_options::skip_permission_denied);
		fs::recursive_directory_iterator copy;
		while (fromroot != end(fromroot))
		{
			++fromroot;
			if (fromroot.depth() > 4)
				break;
		}

		if (fromroot != end(fromroot))
		{
			copy = fromroot;
			CPPUNIT_ASSERT(fromroot.options() == copy.options());
			CPPUNIT_ASSERT(fromroot.depth() == copy.depth());
			CPPUNIT_ASSERT(*fromroot == *copy);
		}
	}

	void iteration()
	{
		std::error_code ec;
		std::set<fs::path> paths;

		if (config::verbose) putchar('\n'); 

		if (config::verbose)
			std::cout << "--------------------------------------------\n";
		fs::recursive_directory_iterator i(
		    "/tmp", fs::directory_options::skip_permission_denied);
		CPPUNIT_ASSERT(i != end(i));

		for (; i != end(i); ++i)
		{
			bool inserted = false;
			if (config::verbose)
				std::cout << " entry: " << i->path().c_str() << '\n';

			std::set<fs::path>::iterator rv;
			std::tie(rv, inserted) = paths.insert(*i);
			CPPUNIT_ASSERT(inserted == true);
		}

		paths.clear();
		if (config::verbose)
			std::cout << "--------------------------------------------\n";
		fs::recursive_directory_iterator j(
		    "/proc", fs::directory_options::skip_permission_denied);
		CPPUNIT_ASSERT(j != end(j));

		for (; j != end(j); ++j)
		{
			bool inserted = false;
			if (config::verbose)
				std::cout << " entry: " << j->path().c_str() << '\n';

			std::set<fs::path>::iterator rv;
			std::tie(rv, inserted) = paths.insert(*j);
			CPPUNIT_ASSERT(inserted == true);
		}

		paths.clear();
		if (config::verbose)
			std::cout << "--------------------------------------------\n";
		fs::recursive_directory_iterator k(
		    "/tmp", fs::directory_options::skip_permission_denied);
		CPPUNIT_ASSERT(k != end(k));

		for (; k != end(k); ++k)
		{
			bool inserted = false;
			if (config::verbose)
				std::cout << " entry: " << k->path().c_str() << '\n';

			std::set<fs::path>::iterator rv;
			std::tie(rv, inserted) = paths.insert(*k);
			CPPUNIT_ASSERT(inserted == true);
		}


		fs::recursive_directory_iterator x = std::move(i);
		CPPUNIT_ASSERT_THROW(++i; , fs::filesystem_error);

		i.increment(ec);

		i = fs::recursive_directory_iterator(fs::path("/tmp"));
		for (auto j = i; j != end(i); ++i)
		{
			CPPUNIT_ASSERT(paths.find(*j) != paths.end());
		}
	}

	void random_tests()
	{
		for (const char * s : { ".", "/tmp", "/dev" })
		{
			std::error_code ec;
			auto di = fs::recursive_directory_iterator(s,
			                fs::directory_options::skip_permission_denied);

			if (config::verbose) putchar('\n');

			for (auto & e : di)
			{
#if 0
				if (config::verbose)
					std::cout << "Found dirent: '" << e.path().c_str()
					          << "\n\tstem = '" << e.path().stem().c_str()
					          << "' ext = '" << e.path().extension().c_str()
					          << "'\n";
#endif

				CPPUNIT_ASSERT(  e.path().filename().string()
				              == ( e.path().stem().string()
				                 + e.path().extension().string()));

				fs::file_status st = e.symlink_status(ec);
				uintmax_t links = fs::hard_link_count(e, ec);
				uintmax_t size = 0;

				if (is_regular_file(st))
				{
					size = fs::file_size(e);
				} else if (fs::is_symlink(e, ec))
				{
					size = 0;
				} else
				{
					CPPUNIT_ASSERT_THROW(fs::file_size(e),
					                     fs::filesystem_error);
				}

				if (config::verbose)
					std::cout << "\tNumber of links: " << links << '\n'
					          << "\tSize: " << size
					          << "\tType: " << (int)st.type() << '\n';


				CPPUNIT_ASSERT(  st.type() == fs::file_type::unknown
				              || st.type() == fs::file_type::none
				              || fs::is_other(st)
				              || fs::is_regular_file(st)
				              || fs::is_directory(st)
				              || fs::is_symlink(st));

				if (  fs::is_block_file(st)
				   || fs::is_character_file(st)
				   || fs::is_fifo(st)
				   || fs::is_socket(st))
					CPPUNIT_ASSERT(fs::is_other(st));

				CPPUNIT_ASSERT(e.path().has_filename());
			}
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_recursive_directory_iterator);
