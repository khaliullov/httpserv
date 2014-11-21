#include <fcntl.h>
#include <unistd.h>
#include <cassert>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

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

bool ready = false;

void client_thread(const net::address & a,
                   const fs::path & p,
                   std::mutex & m,
                   std::condition_variable & cv)
{
	std::unique_lock<std::mutex> ul(m);
	cv.wait(ul, [] { return ready; });
	ul.unlock();

	net::tcp_client_socket c_sock(a);

	c_sock.send_data(p.c_str(), p.string().length());
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

	auto addresses = net::address::get_addresses(SOCK_STREAM,
	                                             "localhost", "2080");

	std::mutex m;
	std::condition_variable cv;

	std::vector<std::thread> threads;

	for (int i = 0; i < 500; ++i)
	{
		size_t idx = i % addresses.size();
		threads.emplace_back(client_thread,
		                     std::ref(addresses.at(idx)),
		                     std::ref(myfile),
		                     std::ref(m),
		                     std::ref(cv));
	}

	m.lock();
	ready = true;
	m.unlock();
	cv.notify_all();
//	net::tcp_client_socket c_sock(address);

//	c_sock.send_data(myfile.c_str(), myfile.string().length());

	for (auto & t : threads)
		t.join();

	return 0;
}

