#include "EventDispatcher.h"
#include "IEventListener.h"
#include "Event.h"

#include <unordered_set>
#include <unordered_map>

struct EventDispatcher::impl
{
	impl(){};
	~impl(){};

	void handleNewListeners();

	bool m_is_dispatching{ false };

	std::unordered_map<EventType, std::unordered_multimap<void*, std::function<void(Event*)>>> m_listeners;
	std::unordered_map<EventType, std::unordered_multimap<void*, std::function<void(Event*)>>> m_listeners_to_add;
	std::unordered_set<void*> m_listeners_to_delete;

	std::unordered_map<EventType, std::unordered_set<IEventListener*>> m_script_listeners;
};

void EventDispatcher::impl::handleNewListeners()
{
	for (auto &listener_to_delete:m_listeners_to_delete)
		for (auto &type_target_callback : m_listeners)
			type_target_callback.second.erase(listener_to_delete);

	for (auto &type_map : m_listeners_to_add)
		for (auto &target_callback : type_map.second)
			m_listeners[type_map.first].emplace(std::move(target_callback));

	m_listeners_to_add.clear();
	m_listeners_to_delete.clear();
}

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
	if (pimpl->m_is_dispatching)
		pimpl->m_listeners_to_add[event_type].emplace(std::move(target), std::move(callback));
	else
		pimpl->m_listeners[event_type].emplace(std::move(target), std::move(callback));
}

//void EventDispatcher::registerListener(EventType event_type, IEventListener *listener)
//{
//	pimpl->m_script_listeners[event_type].emplace(std::move(listener));
//}

void EventDispatcher::deleteListener(void *target)
{
	if (pimpl->m_is_dispatching)
		pimpl->m_listeners_to_delete.emplace(target);
	else
		for (auto &type_target_callback : pimpl->m_listeners)
			type_target_callback.second.erase(target);
}

//void EventDispatcher::deleteListener(IEventListener *listener)
//{
//	for (auto &listener_set : pimpl->m_script_listeners)
//		listener_set.second.erase(listener);
//}

void EventDispatcher::dispatch(std::unique_ptr<Event> &&event, void *target /*= nullptr*/)
{
	if (pimpl->m_is_dispatching)
		throw("Recursive dispatch");
	
	pimpl->handleNewListeners();
	pimpl->m_is_dispatching = true;

	auto event_ptr = event.get();

	if (!target){
		for (auto &target_callback : pimpl->m_listeners[event->getType()])
			target_callback.second(event_ptr);

		//for (auto &listener : pimpl->m_script_listeners[event->getType()])
		//	listener->onEvent(event_ptr);
	}else{
		auto range = pimpl->m_listeners[event->getType()].equal_range(target);
		for (auto target_callback_iter = range.first; target_callback_iter != range.second; ++target_callback_iter)
			target_callback_iter->second(event_ptr);

		//for (auto &listener : pimpl->m_script_listeners[event->getType()])
		//	if (listener == target)
		//		listener->onEvent(event_ptr);
	}

	pimpl->m_is_dispatching = false;
}
