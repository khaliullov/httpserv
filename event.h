#ifndef GUARD_EVENT_H
#define GUARD_EVENT_H 1

#include <sys/epoll.h>

#include <string>
#include <set>
#include <memory>

struct event_listener;

class event_loop
{
 public:
	event_loop(int events = 128, int flags = EPOLL_CLOEXEC);

	~event_loop();

	void add_event(std::shared_ptr<event_listener> & e);

	void delete_event();
	
	void run();

 private:
	int evfd;

	std::set<std::shared_ptr<event_listener>> event_list;

	int max_events;
	std::unique_ptr<struct epoll_event[]> ev_buffer;
};

//////////////////////////////////////////////////////////////////////
struct event_listener
{
	static const uint32_t all_events = (EPOLLIN | EPOLLOUT | EPOLLPRI);
	static const uint32_t all_et_events = (all_events | EPOLLRDHUP | EPOLLET);

	event_listener() { }

	virtual ~event_listener() { }

	event_listener(const event_listener &) = delete;

	event_listener & operator = (const event_listener &) = delete;

	event_listener(event_listener &&) { }

	event_listener & operator = (event_listener &&) { return *this; }

	virtual void action(event_loop & loop, uint32_t events) = 0;

	virtual uint32_t get_default_events() const = 0;

	virtual int descriptor() const = 0;

	static std::string mask_to_string(uint32_t mask);
};

#endif // GUARD_EVENT_H
