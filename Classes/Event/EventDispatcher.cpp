#include "EventDispatcher.h"

#include <unordered_map>

struct EventDispatcher::impl
{
	impl(){};
	~impl(){};

};

EventDispatcher::EventDispatcher() :Object("EventDispatcher"), pimpl(new impl)
{

}

EventDispatcher::~EventDispatcher()
{

}

std::unique_ptr<EventDispatcher> EventDispatcher::create()
{
	return std::unique_ptr<EventDispatcher>(new EventDispatcher);
}

void EventDispatcher::registerListener(Script *script)
{

}

void EventDispatcher::dispatch(std::unique_ptr<Event> &&event)
{

}
