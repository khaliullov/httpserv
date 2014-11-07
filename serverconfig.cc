#include <unistd.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "serverconfig.h"
#include "regexclass.h"

const std::string default_port_number = "2080";

static const struct option longOptions[] = {
	{ "help",       no_argument,        0, 'h' },
	{ "verbose",    no_argument,        0, 'v' },
	{ "listen",     required_argument,  0, 'L' },
	{ "port",       required_argument,  0, 'P' },
	{ 0,            0,                  0, 0   }
};

//////////////////////////////////////////////////////////////////////
void server_config::usage()
{
	const int termWidth = 80;
	char buffer[termWidth + 1];

	int bufLen = 0;
	int prefixLen = snprintf(buffer, termWidth - bufLen, "usage: %s ",
	                         program_name.filename().c_str());

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
  , program_name(static_cast<const char*>(argv[0]))
  , port_number(default_port_number)
  , server_address_list()
{
	int c = -1;
	int longIndex = -1;

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

		 case 'v':
			verbose = true;
			break;

		 case 'P':
			port_number = optarg;
			break;

		 case 'L':
			{
				static const char ip_port_pat[] =
					"^(([0-9.]*)|(\\[([0-9a-zA-Z:]*)\\])):([[:alnum:]]+)$";
				static util::regex re{ip_port_pat};

				std::string subject(optarg);
				std::string addr(subject);
				auto matches = re.match(subject);
				std::vector<net::address> tmplist;

				if ( ! matches.empty() )
				{
					int b, e;

					if (matches[2].rm_so != -1 && matches[2].rm_eo != -1)
					{
						b = matches[2].rm_so;
						e = matches[2].rm_eo;
						addr = subject.substr(b, e - b);
					} else if (matches[4].rm_so != -1 && matches[4].rm_eo != -1)
					{
						b = matches[4].rm_so;
						e = matches[4].rm_eo;
						addr = subject.substr(b, e - b);
					} else
					{
						throw std::runtime_error("Bad -L argument");
					}

					b = matches[5].rm_so;
					e = matches[5].rm_eo;
					std::string port = subject.substr(b, e - b);

					if (addr.empty())
						throw std::runtime_error("Bad listen address");

					tmplist = net::address::get_addresses(SOCK_STREAM, addr,
					                                      port);
				} else
				{
					tmplist = net::address::get_addresses(SOCK_STREAM, addr,
					                                      port_number);
				}

				for (auto && elem : tmplist)
					server_address_list.emplace_back(elem);
			}
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

	if (server_address_list.empty())
		server_address_list =
			net::address::passive_addresses(SOCK_STREAM, port_number);
}
