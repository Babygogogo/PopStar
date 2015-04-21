#include "EventDispatcher.h"
#include "Event.h"
#include "../Common/IEventListener.h"

#include <unordered_set>
#include <unordered_map>

struct EventDispatcher::impl
{
	impl(){};
	~impl(){};

	std::unordered_map<EventType, std::unordered_multimap<void*, std::function<void(Event*)>>> m_listeners;
	std::unordered_map<EventType, std::unordered_set<IEventListener*>> m_script_listeners;
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

void EventDispatcher::registerListener(EventType event_type, void *target, std::function<void(Event*)> callback)
{
	pimpl->m_listeners[event_type].emplace(std::move(target), std::move(callback));
}

void EventDispatcher::registerListener(EventType event_type, IEventListener *listener)
{
	pimpl->m_script_listeners[event_type].emplace(std::move(listener));
}

void EventDispatcher::deleteListener(void *target)
{
	for (auto &type_target_callback : pimpl->m_listeners)
		type_target_callback.second.erase(target);
}

void EventDispatcher::deleteListener(IEventListener *listener)
{
	for (auto &listener_set : pimpl->m_script_listeners)
		listener_set.second.erase(listener);
}

void EventDispatcher::dispatch(std::unique_ptr<Event> &&event)
{
	auto event_ptr = event.get();

	for (auto &target_callback : pimpl->m_listeners[event->getType()])
		target_callback.second(event_ptr);

	for (auto &listener : pimpl->m_script_listeners[event->getType()])
		listener->onEvent(event_ptr);
}
