#ifndef GUARD_HTTP_PARSER_WRAPPER
#define GUARD_HTTP_PARSER_WRAPPER 1

#include <string>
#include "http_parser.h"

class http_server
{
 public:
	http_server();

	http_server(http_server &&);
	http_server & operator = (http_server &&);

	http_server(const http_server &) = delete;
	http_server & operator = (const http_server &) = delete;

	virtual ~http_server();

	virtual int message_start();

	virtual int process_url(const char * data, size_t length);

	virtual int header_name(const char * data, size_t length);

	virtual int header_value(const char * data, size_t length);

	virtual int headers_complete();

	virtual int body_data(const char * data, size_t length); 

	virtual int message_complete();

 protected:
	struct http_parser parser;
	struct http_parser_settings callbacks;
};

#endif // GUARD_HTTP_PARSER_WRAPPER
