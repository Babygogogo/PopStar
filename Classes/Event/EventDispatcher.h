#ifndef __EVENT_DISPATCHER__
#define __EVENT_DISPATCHER__

#include <memory>

#include "Event.h"
#include "../Script/Script.h"

class EventDispatcher final : public Object
{
public:
	static std::unique_ptr<EventDispatcher> create();
	~EventDispatcher();

	void registerListener(Script *script);
	void dispatch(std::unique_ptr<Event> &&event);	

private:
	EventDispatcher();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__EVENT_DISPATCHER__
