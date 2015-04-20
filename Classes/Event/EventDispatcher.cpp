#include "EventDispatcher.h"
#include "../Common/IEventListener.h"

#include <unordered_set>
#include <unordered_map>

struct EventDispatcher::impl
{
	impl(){};
	~impl(){};

	std::unordered_map<EventType, std::unordered_map<void*, std::function<void()>>> m_listeners;
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

void EventDispatcher::registerListener(EventType event_type, void *target, std::function<void()> callback)
{
	pimpl->m_listeners[event_type][target] = std::move(callback);
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
	for (auto &target_callback : pimpl->m_listeners[event->getType()])
		target_callback.second();

	for (auto &listener : pimpl->m_script_listeners[event->getType()])
		listener->onEvent(event.get());
}
