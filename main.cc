#include <cstdio>
#include <unistd.h>

#include "serverconfig.h"

int main(int argc, char ** argv)
{
	ServerConfig config(argc, argv);
	return 0;
}
