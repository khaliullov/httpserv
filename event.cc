
#include "event.h"

#include <unistd.h>

#include <cstring>

#include "utility/util.h"

event_loop::event_loop(int events, int flags)
  : evfd(epoll_create1(flags))
  , max_events(events)
  , ev_buffer(new epoll_event[events])
	{ if (evfd < 0) throw make_syserr("epoll_create() failed"); }

event_loop::~event_loop()
	{ if (evfd >= 0) close(evfd); }

void event_loop::add_event(std::shared_ptr<event_listener> & e)
{
	event_list.insert(e);

	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = e->get_default_events();
	ev.data.ptr = e.get();

	epoll_ctl(evfd, EPOLL_CTL_ADD, e->descriptor(),  &ev);
}

void event_loop::run()
{
	int count = 0;
	while ((count = epoll_wait(evfd, ev_buffer.get(), max_events, -1)) > 0)
	{
//		printf("%s: Read %d events\n", __func__, count);

		for (int i = 0; i < count; ++i)
		{
			event_listener * listener =
			  reinterpret_cast<event_listener*>(ev_buffer[i].data.ptr);

			listener->action(*this, ev_buffer[i].events);
		}
//		printf("Waiting for event...\n");
	}
}

