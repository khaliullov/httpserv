#ifndef GUARD_INTERACTIVE_LISTENER_H
#define GUARD_INTERACTIVE_LISTENER_H 1

#include "event.h"

//////////////////////////////////////////////////////////////////////
struct interactive_listener : public event_listener
{
 public:
	interactive_listener(int input_fd);

	virtual ~interactive_listener();

	void action(event_loop &, uint32_t events) override;

	uint32_t get_default_events() const override;

	int descriptor() const override;

 protected:
	int infd;
};

#endif // GUARD_INTERACTIVE_LISTENER_H
