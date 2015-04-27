#ifndef __EVENT_DISPATCHER__
#define __EVENT_DISPATCHER__

#include <memory>
#include <functional>

#include "../Common/Object.h"

class IEventListener;
class Event;
enum class EventType;

class EventDispatcher final : public Object
{
	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;
	EventDispatcher& operator=(EventDispatcher&&) = delete;
	
public:
	static std::unique_ptr<EventDispatcher> create();
	~EventDispatcher();

	void registerListener(EventType event_type, void *target, std::function<void(Event*)> callback);
	void registerListener(EventType event_type, IEventListener *listener);
	void deleteListener(void *target);
	void deleteListener(IEventListener *listener);

	void dispatch(std::unique_ptr<Event> &&event, void *target = nullptr);	

private:
	EventDispatcher();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__EVENT_DISPATCHER__
