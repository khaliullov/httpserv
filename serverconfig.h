#ifndef GUARD_SERVERCONFIG_H
#define GUARD_CONFIG_H 1

#include <deque>
#include <string>

//////////////////////////////////////////////////////////////////////
class ServerConfig
{
 public:
	ServerConfig(int argc, char ** argv);

	virtual ~ServerConfig() { }

	virtual void usage();

	bool verbose;
	std::string programName;
	std::string serverAddr;
	std::string serverPort;
};

#endif // GUARD_SERVERCONFIG_H
