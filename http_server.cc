#include "http_server.h"
#include <cstring>

extern "C" {
	static
	int cb_message_start(http_parser * p)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->message_start();
	}

	static
	int cb_process_url(http_parser * p, const char * data, size_t length)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->process_url(data, length);
	}

	static
	int cb_header_name(http_parser * p, const char * data, size_t length)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->header_name(data, length);
	}

	static
	int cb_header_value(http_parser * p, const char * data, size_t length)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->header_value(data, length);
	}

	static
	int cb_headers_complete(http_parser * p)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->headers_complete();
	}

	static
	int cb_body_data(http_parser * p, const char * data, size_t length)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->body_data(data, length);
	}

	static
	int cb_message_complete(http_parser * p)
	{
		http_server * s = reinterpret_cast<http_server*>(p->data);
		return s->message_complete();
	}

} // extern "C"

http_server::http_server()
  : parser()
  , callbacks()
{
	memset(&parser, 0, sizeof(parser));
	http_parser_init(&parser, HTTP_REQUEST);
	parser.data = this;

	callbacks.on_message_begin = cb_message_start;
	callbacks.on_url = cb_process_url;
	callbacks.on_header_field = cb_header_name;
	callbacks.on_header_value = cb_header_value;
	callbacks.on_headers_complete = cb_headers_complete;
	callbacks.on_body = cb_body_data;
	callbacks.on_message_complete = cb_message_complete;
}

http_server::~http_server()
{
}

int http_server::message_start()
{
	printf("Start of message\n");
	return 0;
}

int http_server::process_url(const char * data, size_t length)
{
	std::string s(data, length);
	printf("%s: data = %s\n", __func__, s.c_str());
	return 0;
}

int http_server::header_name(const char * data, size_t length) 
{
	std::string s(data, length);
	printf("%s: data = %s\n", __func__, s.c_str());
	return 0;
}

int http_server::header_value(const char * data, size_t length) 
{
	std::string s(data, length);
	printf("%s: data = %s\n", __func__, s.c_str());
	return 0;
}

int http_server::headers_complete() 
{
	printf("End of headers\n");
	return 0;
}

int http_server::body_data(const char * data, size_t length) 
{
	std::string s(data, length);
	printf("%s: data = %s\n", __func__, s.c_str());
	return 0;
}

int http_server::message_complete() 
{
	printf("End of message\n");
	return 0;
}
