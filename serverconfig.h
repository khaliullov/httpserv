#ifndef GUARD_SERVERCONFIG_H
#define GUARD_CONFIG_H 1

#include <vector>
#include <string>
#include "filesystem/filesystem"
#include "address.h"

//////////////////////////////////////////////////////////////////////
class server_config
{
 public:
	server_config(int argc, char ** argv);

	virtual ~server_config() { }

	virtual void usage();

	bool verbose;
	filesystem::path program_name;

	std::string port_number;
	std::vector<net::address> server_address_list;
};

#endif // GUARD_SERVERCONFIG_H
