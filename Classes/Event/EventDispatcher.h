#ifndef __EVENT_DISPATCHER__
#define __EVENT_DISPATCHER__

#include <memory>
#include <functional>

#include "Event.h"
#include "../Script/Script.h"

class EventDispatcher final : public Object
{
public:
	static std::unique_ptr<EventDispatcher> create();
	~EventDispatcher();

	void registerListener(EventType event_type, void *target, std::function<void()> callback);
	void registerListener(EventType event_type, Script *listener);
	void deleteListener(void *target);
	void deleteListener(Script *listener);

	void dispatch(std::unique_ptr<Event> &&event);	

private:
	EventDispatcher();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__EVENT_DISPATCHER__
