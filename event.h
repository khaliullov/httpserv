#ifndef GUARD_EVENT_H
#define GUARD_EVENT_H 1

#include <sys/epoll.h>

#include <string>
#include <set>
#include <memory>

#include "utility/util.h"

class event_listener;

class event_loop
{
 public:
	event_loop(int events = 128, int flags = EPOLL_CLOEXEC);

	~event_loop();

	void add_event(std::shared_ptr<event_listener> & e);

	void delete_event(const std::shared_ptr<event_listener> & e);
	
	void run();

 private:
	int evfd;

	std::set<std::shared_ptr<event_listener>> event_list;

	int max_events;
	std::unique_ptr<struct epoll_event[]> ev_buffer;
};

//////////////////////////////////////////////////////////////////////
class event_listener : public std::enable_shared_from_this<event_listener>
{
 public:
	static const uint32_t all_events =
	  (EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLRDHUP);
	static const uint32_t all_input_events = (EPOLLIN | EPOLLPRI | EPOLLRDHUP);
	static const uint32_t all_et_events = (all_events | EPOLLET);
	static const uint32_t all_et_input_events = (all_input_events | EPOLLET);

	event_listener()
	  : std::enable_shared_from_this<event_listener>()
	{ }

	virtual ~event_listener() { }

	event_listener(const event_listener &) = delete;

	event_listener & operator = (const event_listener &) = delete;

	event_listener(event_listener &&) { }

	event_listener & operator = (event_listener &&) { return *this; }

	virtual void action(event_loop & loop, uint32_t events) = 0;

	virtual uint32_t get_default_events() const = 0;

	virtual int descriptor() const = 0;

	static std::string mask_to_string(uint32_t mask);

	std::shared_ptr<event_listener> get_shared()
	{ return shared_from_this(); }
};

#endif // GUARD_EVENT_H
