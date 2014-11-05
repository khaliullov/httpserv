#ifndef GUARD_SERVERCONFIG_H
#define GUARD_CONFIG_H 1

#include <deque>
#include <string>
#include "filesystem/filesystem"

//////////////////////////////////////////////////////////////////////
class server_config
{
 public:
	server_config(int argc, char ** argv);

	virtual ~server_config() { }

	virtual void usage();

	bool verbose;
	filesystem::path programName;
	std::string serverAddr;
	std::string serverPort;
};

#endif // GUARD_SERVERCONFIG_H
