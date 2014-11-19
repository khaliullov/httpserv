#include "filesystem/path.h"
#include "filesystem/file_status.h"
#include "filesystem/fs_operations.h"
#include "filesystem/directory_iterator.h"

#include <cstdio>
#include <iostream>
#include <set>

#include "cppunit-header.h"

namespace fs = filesystem::v1;

class Test_directory_iterator : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_directory_iterator);
	CPPUNIT_TEST(constructors);
	CPPUNIT_TEST(assignment);
	CPPUNIT_TEST(iteration);
	CPPUNIT_TEST(random_tests);
	CPPUNIT_TEST_SUITE_END();

 protected:
	void constructors()
	{
		std::error_code ec;
		fs::directory_iterator empty;

		CPPUNIT_ASSERT(empty == end(fs::directory_iterator()));

		CPPUNIT_ASSERT_NO_THROW(fs::directory_iterator i;);
		CPPUNIT_ASSERT_NO_THROW(fs::directory_iterator i(fs::path{"/tmp"}););
		CPPUNIT_ASSERT_NO_THROW(
		   fs::directory_iterator i(fs::path{"/tmp"}, ec););

		CPPUNIT_ASSERT_THROW({fs::directory_iterator j(fs::path("/foo"));},
		                     fs::filesystem_error);

		CPPUNIT_ASSERT_THROW({fs::directory_iterator j(fs::path{"/root"});},
		                     fs::filesystem_error);

		CPPUNIT_ASSERT_THROW({fs::directory_iterator j(fs::path{""});},
		                     fs::filesystem_error);

		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::directory_iterator j(fs::path("/foo"), ec););
		CPPUNIT_ASSERT(ec);
		               
		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::directory_iterator j(fs::path("/root"), ec););
		CPPUNIT_ASSERT(ec);
		               
		ec.clear();
		CPPUNIT_ASSERT_NO_THROW(
			fs::directory_iterator j(fs::path(""), ec););
		CPPUNIT_ASSERT(ec);
	}

	void assignment()
	{
		fs::directory_iterator i("/tmp");
		fs::directory_iterator j("/tmp");
		fs::directory_iterator empty;

		i = fs::directory_iterator{};
		i = j;
		i = empty;
	}

	void iteration()
	{
		std::error_code ec;
		std::set<fs::path> paths;

		if (config::verbose)
			putchar('\n'); 

		fs::directory_iterator i(fs::path("/tmp"));
		CPPUNIT_ASSERT(i != end(i));

		for (; i != end(i); ++i)
		{
			if (config::verbose)
				std::cout << " entry of /tmp: " << i->path().c_str() << '\n';

			std::set<fs::path>::iterator rv;
			bool inserted = false;

			std::tie(rv, inserted) = paths.insert(*i);

			CPPUNIT_ASSERT(inserted == true);
		}

		fs::directory_iterator x = std::move(i);
		CPPUNIT_ASSERT_THROW(++i; , fs::filesystem_error);

		i.increment(ec);

		i = fs::directory_iterator(fs::path("/tmp"));
		for (auto & j : i)
		{
			CPPUNIT_ASSERT(paths.find(j) != paths.end());
		}
	}

	void random_tests()
	{
		for (const char * s : { ".", "/tmp", "/dev" })
		{
			fs::directory_iterator di(s);

			if (config::verbose) putchar('\n');

			for (auto & e : di)
			{
				if (config::verbose)
					std::cout << "Found dirent: '" << e.path().c_str()
					          << "\n\tstem = '" << e.path().stem().c_str()
					          << "' ext = '" << e.path().extension().c_str()
					          << "'\n";

				CPPUNIT_ASSERT(  e.path().filename().string()
				              == ( e.path().stem().string()
				                 + e.path().extension().string()));

				fs::file_status st = e.symlink_status();
				uintmax_t links = fs::hard_link_count(e);
				uintmax_t size = 0;

				if (is_regular_file(st))
				{
					size = fs::file_size(e);
				} else if (fs::is_symlink(e))
				{
					size = 0;
				} else
				{
					CPPUNIT_ASSERT_THROW(fs::file_size(e),
					                     fs::filesystem_error);
				}

				if (config::verbose)
					std::cout << "\tNumber of links: " << links << '\n'
					          << "\tSize: " << size << '\n';

				CPPUNIT_ASSERT(fs::is_other(st)
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

CPPUNIT_TEST_SUITE_REGISTRATION(Test_directory_iterator);
