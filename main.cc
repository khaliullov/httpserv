#include <cstdio>
#include <unistd.h>

#include "serverconfig.h"

int main(int argc, char ** argv)
{
	server_config config(argc, argv);

	return 0;
}
