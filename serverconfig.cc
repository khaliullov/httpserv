#include <unistd.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "serverconfig.h"

static const struct option longOptions[] = {
	{ "help",       no_argument,        0, 'h' },
	{ "port",       required_argument,  0, 'P' },
	{ "server",     required_argument,  0, 'S' },
	{ "verbose",    no_argument,        0, 'v' },
	{ 0,            0,                  0, 0   }
};

//////////////////////////////////////////////////////////////////////
void server_config::usage()
{
	const int termWidth = 80;
	char buffer[termWidth + 1];

	int bufLen = 0;
	int prefixLen = snprintf(buffer, termWidth - bufLen, "usage: %s ",
	                         programName.c_str());

	bufLen += prefixLen;

	for (int i = 0; longOptions[i].name != 0 || longOptions[i].val != 0; ++i)
	{
		bool again = false;
		do {
			again = false;
			int oldLen = bufLen;
			if (  longOptions[i].has_arg == required_argument
			   && longOptions[i].val != 0)
			{
				bufLen += snprintf(buffer + bufLen, termWidth - bufLen,
				                   "[--%s|-%c <%s>] ", longOptions[i].name,
				                   longOptions[i].val, longOptions[i].name);
			} else if (  longOptions[i].has_arg == required_argument
			          && longOptions[i].val == 0)
			{
				bufLen += snprintf(buffer + bufLen, termWidth - bufLen,
				                   "[--%s <%s>] ", longOptions[i].name,
				                   longOptions[i].name);
			} else if (longOptions[i].has_arg == no_argument)
			{
				bufLen += snprintf(buffer + bufLen, termWidth - bufLen,
				                   "[--%s|-%c] ", longOptions[i].name,
				                   longOptions[i].val);
			}

			if (bufLen >= termWidth)
			{
				buffer[oldLen] = '\0';
				printf("%s\n", buffer);
				bufLen = snprintf(buffer, termWidth, "%*s", prefixLen, " ");
				again = true;
			}
		} while (again);
	}

	buffer[bufLen] = '\n';
	buffer[bufLen + 1] = '\0';

	printf("%s\n", buffer);
}

//////////////////////////////////////////////////////////////////////
server_config::server_config(int argc, char ** argv)
  : verbose(false)
  , programName(argv[0])
  , serverAddr("127.0.0.1")
  , serverPort("8080")
{
	int c = -1;
	int longIndex = -1;

	std::string::size_type x = programName.find_last_of('/');
	if (x != std::string::npos)
		programName = programName.substr(x + 1);

	// generate short option list
	std::string shortOpts{":"};
	for (int i = 0; longOptions[i].name != 0 || longOptions[i].val != 0; ++i)
	{
		if (longOptions[i].val != 0)
		{
			shortOpts += static_cast<char>(longOptions[i].val);

			if (longOptions[i].has_arg == required_argument)
				shortOpts += ':';
			else if (longOptions[i].has_arg == optional_argument)
				shortOpts += "::";
		}
	}

	while ((c = getopt_long(argc, argv, shortOpts.c_str(),
	                        longOptions, &longIndex)) > 0)
	{
		switch (c)
		{
		 case 'h':
			usage();
			exit(0);
			break;

		 case 'P':
			serverPort = optarg;
			break;

		 case 'S':
			serverAddr = optarg;
			break;

		 case 'v':
			verbose = true;
			break;

		 case ':':
			{
			std::string msg{"Option requires argument: "};
			if (argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-')
				msg += argv[optind - 1];
			else
				msg += std::string("-") + static_cast<char>(optopt);

			throw std::runtime_error(msg);

			}
			break;

		 case '?':
		 default:
			{
			std::string msg{"Bad option: "};
			if (argv[optind - 1][0] == '-' && argv[optind - 1][1] == '-')
				msg += argv[optind - 1];
			else
				msg += static_cast<char>(optopt);
			throw std::runtime_error(msg);

			}
			break;
		}
		longIndex = -1;
	}
}
