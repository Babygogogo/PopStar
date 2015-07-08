#include <unordered_set>
#include <unordered_map>
#include <list>
#include <cassert>

#include "EventDispatcher.h"
#include "IEventListener.h"
#include "LegacyEvent.h"
#include "EventType.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//definition of EventDispatcherImpl
//////////////////////////////////////////////////////////////////////////
struct EventDispatcher::EventDispatcherImpl
{
	EventDispatcherImpl(){};
	~EventDispatcherImpl(){};

	void handleNewListeners();

	//Determine if we can add a listener and callback pair into the listener list.
	bool canAddListenerCallback(const EventType & eType, const ListenerCallback & eListenerCallback) const;

	//Check if there is a same listener to an event type already.
	bool hasSameListener(const EventType & eType, const std::shared_ptr<void> & eListener) const;

	bool m_is_dispatching{ false };

	std::unordered_map<LegacyEventType, std::unordered_multimap<void*, std::function<void(LegacyEvent*)>>> m_listeners;
	std::unordered_map<LegacyEventType, std::unordered_multimap<void*, std::function<void(LegacyEvent*)>>> m_listeners_to_add;
	std::unordered_set<void*> m_listeners_to_delete;

	std::unordered_map<LegacyEventType, std::unordered_set<IEventListener*>> m_script_listeners;

	std::unordered_map<EventType, std::list<ListenerCallback>> m_Listeners;
};

void EventDispatcher::EventDispatcherImpl::handleNewListeners()
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

bool EventDispatcher::EventDispatcherImpl::canAddListenerCallback(const EventType & eType, const ListenerCallback & eListenerCallback) const
{
	//If the new listener is already dead, we can't add it to the listener list.
	if (eListenerCallback.first.expired())
		return false;

	//If there is a same listener to the same event type, we can't add it to the list either.
	if (hasSameListener(eType, eListenerCallback.first.lock())){
		cocos2d::log("EventDispatcher::vAddListener trying to double-register a listener.");
		return false;
	}

	return true;
}

bool EventDispatcher::EventDispatcherImpl::hasSameListener(const EventType & eType, const std::shared_ptr<void> & eListener) const
{
	//If there is no event type the same as eType in the list, there is of course no the same listener in it.
	auto typeIter = m_Listeners.find(eType);
	if (typeIter == m_Listeners.end())
		return false;

	//Traverse the listener list to see if there is a same listener.
	for (const auto &listenerInList : typeIter->second){
		//If the current listener is dead, simply ignore it.
		if (listenerInList.first.expired())
			continue;

		//If the current listener is the same as eListener, return true.
		if (listenerInList.first.lock() == eListener)
			return true;
	}

	//There is no the same listener in the list. Return false.
	return false;
}

//////////////////////////////////////////////////////////////////////////
//implementation of EventDispatcher
//////////////////////////////////////////////////////////////////////////
EventDispatcher::EventDispatcher() : pimpl(new EventDispatcherImpl)
{

}

EventDispatcher::~EventDispatcher()
{

}

std::unique_ptr<EventDispatcher> EventDispatcher::create()
{
	return std::unique_ptr<EventDispatcher>(new EventDispatcher);
}

void EventDispatcher::registerListener(LegacyEventType event_type, void *target, std::function<void(LegacyEvent*)> callback)
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

void EventDispatcher::dispatch(std::unique_ptr<LegacyEvent> &&event, void *target /*= nullptr*/)
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

void EventDispatcher::vAddListener(const EventType & eType, const ListenerCallback & eListenerCallback)
{
	if (pimpl->canAddListenerCallback(eType, eListenerCallback))
		pimpl->m_Listeners[eType].emplace_back(eListenerCallback);
}

void EventDispatcher::vAddListener(const EventType & eType, ListenerCallback && eListenerCallback)
{
	if (pimpl->canAddListenerCallback(eType, eListenerCallback))
		pimpl->m_Listeners[eType].emplace_back(std::move(eListenerCallback));
}

void EventDispatcher::vRemoveListener(const EventType & eType, const std::weak_ptr<void> & eListener)
{
	//If the eListener is dead or there is no eType in the listener list, simply return.
	if (eListener.expired() || pimpl->m_Listeners.find(eType) == pimpl->m_Listeners.end())
		return;

	//Prepare for traversing the listener list.
	const auto strongListener = eListener.lock();
	auto & listenerListOfType = pimpl->m_Listeners[eType];

	//Traverse the listener list to see if there is an listener to be removed.
	//Don't use for(xx:yy) because there is at most one listener to remove from the list. Once we find it, we can simply remove it and return.
	for (auto listenerIter = listenerListOfType.begin(); listenerIter != listenerListOfType.end(); ++listenerIter){
		//If the current listener is dead, ignore it.
		if (listenerIter->first.expired())
			continue;

		//If the current listener "equals" eListener, remove it from the list.
		if (listenerIter->first.lock() == strongListener){
			listenerListOfType.erase(listenerIter);
			return;
		}
	}
}

void EventDispatcher::vQueueEvent(const std::shared_ptr<IEventData> & eData)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void EventDispatcher::vQueueEvent(std::shared_ptr<IEventData> && eData)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void EventDispatcher::vAbortEvent(const EventType & eType, bool allOfThisType /*= false*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void EventDispatcher::vTrigger(const std::shared_ptr<IEventData> & eData)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void EventDispatcher::vTrigger(std::shared_ptr<IEventData> && eData)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void EventDispatcher::vDispatchQueuedEvents(time_t timeOutMs /*= 10*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}
