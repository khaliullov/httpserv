
#include "event.h"

#include <unistd.h>

#include <cstring>
#include <mutex>

#include "utility/util.h"

event_loop::event_loop(int events, int flags)
  : evfd(epoll_create1(flags))
  , lock()
  , event_list()
  , max_events(events)
  , ev_buffer(new epoll_event[events])
	{ if (evfd < 0) throw make_syserr("epoll_create() failed"); }

event_loop::~event_loop()
{
	std::lock_guard<spin_lock> lh(lock);
	if (evfd >= 0) close(evfd);
}

void event_loop::add_event(std::shared_ptr<event_listener> & e)
{
	std::lock_guard<spin_lock> lh(lock);
	event_list.insert(e);

	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = e->get_default_events();
	ev.data.ptr = e.get();

	epoll_ctl(evfd, EPOLL_CTL_ADD, e->descriptor(),  &ev);
}

void event_loop::delete_event(const std::shared_ptr<event_listener> & e)
{
	std::lock_guard<spin_lock> lh(lock);

	if (epoll_ctl(evfd, EPOLL_CTL_DEL, e->descriptor(), nullptr) != 0)
		throw make_syserr("Could not delete epoll listener");

	if (event_list.erase(e) <= 0)
		throw make_syserr("Could not find event listener");

}
	
void event_loop::run(int id)
{
	int count = 0;
	while ((count = epoll_wait(evfd, ev_buffer.get(), max_events, -1)) > 0)
	{
		printf("%s [%d]: Read %d events\n", __func__, id, count);

		for (int i = 0; i < count; ++i)
		{
			event_listener * ptr =
			  reinterpret_cast<event_listener*>(ev_buffer[i].data.ptr);

			std::unique_lock<spin_lock> lh(lock);
			auto listener = ptr->get_shared();
			lh.unlock();

			if (listener)
				listener->action(*this, ev_buffer[i].events);
		}
//		printf("Waiting for event...\n");
	}
}

std::string event_listener::mask_to_string(uint32_t mask)
{
	std::string str{"[ "};

	if (mask & EPOLLIN)
		str += "IN ";
	if (mask & EPOLLOUT)
		str += "OUT ";
	if (mask & EPOLLRDHUP)
		str += "RDHUP ";
	if (mask & EPOLLPRI)
		str += "PRI ";
	if (mask & EPOLLERR)
		str += "ERR ";
	if (mask & EPOLLHUP)
		str += "HUP ";
	if (mask & EPOLLET)
		str += "ET ";
	if (mask & EPOLLONESHOT)
		str += "ONESHOT ";

	str += ']';
	return str;
}
