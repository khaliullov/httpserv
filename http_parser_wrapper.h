#ifndef GUARD_HTTP_PARSER_WRAPPER
#define GUARD_HTTP_PARSER_WRAPPER 1

#include <string>

class http_server
{

	virtual void message_start()
	{ printf("Start of message\n"); }

	virtual void process_url(const char * data, size_t length)
	{
		std::string s(data, length);
		printf("%s: data = %s\n", __func__, s.c_str());
	}

	virtual void header_name(const char * data, size_t length) 
	{
		std::string s(data, length);
		printf("%s: data = %s\n", __func__, s.c_str());
	}

	virtual void header_value(const char * data, size_t length) 
	{
		std::string s(data, length);
		printf("%s: data = %s\n", __func__, s.c_str());
	}

	virtual void headers_complete() 
	{ printf("End of headers\n"); }

	virtual void body_data(const char * data, size_t length) 
	{
		std::string s(data, length);
		printf("%s: data = %s\n", __func__, s.c_str());
	}

	virtual void message_complete() 
	{ printf("End of message\n"); }
};

#endif // GUARD_HTTP_PARSER_WRAPPER
