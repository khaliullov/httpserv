#include <unistd.h>

#include <cstdio>
#include <string>

#include <readline/readline.h>

#include "interactive_listener.h"

extern "C" {

static void readlineCB(char * s)
{
	if (s != nullptr)
	{
		std::string cmd(s);
		printf("Read command: '%s'\n", s);
		free(s);
		if (cmd == "quit")
			exit(0);
	} else
	{
		exit(0);
	}
}

} // extern "C"

interactive_listener::interactive_listener(int input_fd)
  : infd(input_fd)
{
	rl_callback_handler_install("statserv> ", readlineCB);
}

interactive_listener::~interactive_listener()
{
	rl_callback_handler_remove();
	close(infd);
}

void interactive_listener::action(event_loop &, uint32_t events)
{
	if (events & EPOLLIN)
	{
		rl_callback_read_char();
	}
}

uint32_t interactive_listener::get_default_events() const
	{ return (EPOLLIN); }

int interactive_listener::descriptor() const
	{ return infd; }
