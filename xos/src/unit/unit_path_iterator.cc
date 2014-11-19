
#include "filesystem/path.h"
#include "filesystem/path_iterator.h"

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>

#include "cppunit-header.h"

class Test_Path_Iterator : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(Test_Path_Iterator);
	CPPUNIT_TEST(testIncrementDecrement);
	CPPUNIT_TEST_SUITE_END();

 protected:
	// FIXME - This test needs work
	void testIncrementDecrement()
	{
		using namespace filesystem;

		std::vector<std::string> values {
			{ "" },
			{ "/" },
			{ "/////" },
			{ "foo" },
			{ "foo/" },
			{ "foo/////" },
			{ "/foo" },
			{ "/foo/" },
			{ "foo/bar" },
			{ "/foo/bar" },
			{ "/foo/bar/" },
			{ "/a/b/c/d/e/f" },
			{ "/a/b/c/d/e/f/" },
			{ "a/b/c/d/e/f" },
			{ "a/b/c/d/e/f/" },
		};

		if (config::verbose) putchar('\n');
		for (auto & s : values)
		{
			if (config::verbose)
				std::cout << "Iterating through '" << s << "'\n";

			path p(s);

			path::iterator i(p.begin());
			path::iterator j;

			j = i;

			CPPUNIT_ASSERT(j == i);
			++j;
			CPPUNIT_ASSERT(j != i);
			j = std::move(i);

			path::iterator k(std::move(j));

			i = k;

			for (auto e = p.begin(); e != p.end(); e++, ++i)
			{
				if (config::verbose)
				{
					std::cout << (*e).c_str() << std::endl;
				}
				CPPUNIT_ASSERT((*e).generic_string() == i->generic_string());
			}

			for (auto x = --p.end(); x != --p.begin(); x--)
			{
				if (config::verbose)
					std::cout << x->c_str() << std::endl;
			}
			if (config::verbose)
			{
				std::cout << p.parent_path().c_str() << std::endl;
				std::cout << "--------------------------------\n";
			}
		}
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION(Test_Path_Iterator);
