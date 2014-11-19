#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <string>

#include "filesystem/filesystem"
#include "utility/crc.h"
#include "address.h"
#include "tcp_client_socket.h"

namespace fs = filesystem;

uint32_t getFileCrc(const fs::path & filename)
{
	uint32_t crc = 0;
	CRC_32 accum;
	const int bufsz = 4096;
	char buffer[bufsz];
	int fd;
	int n;

	if ((fd = open(filename.c_str(), O_RDONLY)) < 0)
		throw make_syserr("Could not open file");

	while ((n = read(fd, buffer, bufsz)) > 0)
	{
		accum(buffer, n);
	}

	if (n < 0) throw std::runtime_error("read failed");

	close(fd);

	crc = accum.get();

	return crc;
}
	

int main(int argc, char ** argv)
{
	fs::path prog_name{std::string(argv[0])};
	if (argc < 2)
		printf("usage: %s <file>\n", prog_name.filename().c_str());

	fs::path myfile{std::string(argv[1])};

	myfile = fs::canonical(myfile, fs::current_path());

	auto size = fs::file_size(myfile);

	if (size <= 0)
		throw std::runtime_error("Bad file size");

	printf("crc read from file '%s' = %08x\n", myfile.c_str(),
	       getFileCrc(myfile));

	auto address = net::address::get_addresses(SOCK_STREAM,
	                                           "localhost", "2080").front();


	net::tcp_client_socket c_sock(address);

	c_sock.send_data(myfile.c_str(), myfile.string().length());

	return 0;
}

