#include "filesystem/filesystem"
#include "address.h"
#include "tcp_client_socket.h"

namespace fs = filesystem;

int main(int argc, char ** argv)
{
	fs::path prog_name(argv[0]);
	if (argc < 2)
		printf("usage: %s <file>\n", prog_name.filename().c_str());

	return 0;
}

