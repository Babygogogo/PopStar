#ifndef __EVENT_DISPATCHER__
#define __EVENT_DISPATCHER__

#include <memory>
#include <functional>

#include "IEventDispatcher.h"

class IEventListener;
class Event;
enum class LegacyEventType;

class EventDispatcher final : public IEventDispatcher
{
public:
	~EventDispatcher();

	static std::unique_ptr<EventDispatcher> create();

	void registerListener(LegacyEventType event_type, void *target, std::function<void(Event*)> callback);
//	void registerListener(EventType event_type, IEventListener *listener);
	void deleteListener(void *target);
//	void deleteListener(IEventListener *listener);

	void dispatch(std::unique_ptr<Event> &&event, void *target = nullptr);	

	//Disable copy/move constructor and operator=.
	EventDispatcher(const EventDispatcher&) = delete;
	EventDispatcher(EventDispatcher&&) = delete;
	EventDispatcher& operator=(const EventDispatcher&) = delete;
	EventDispatcher& operator=(EventDispatcher&&) = delete;

private:
	EventDispatcher();

	//override functions of the interface
	virtual void vAddListener(const EventType & eType, const std::weak_ptr<IEventListener> & eListener) override;
	virtual void vAddListener(const EventType & eType, std::weak_ptr<IEventListener> && eListener) override;
	virtual void vRemoveListener(const EventType & eType, const std::weak_ptr<IEventListener> & eListener) override;
	virtual void vQueueEvent(std::shared_ptr<IEventData> eData) override;
	virtual void vAbortEvent(const EventType & eType, bool allOfThisType = false) override;
	virtual void vTrigger(std::shared_ptr<IEventData> eData) override;
	virtual void vDispatchQueuedEvents(time_t timeOutMs = 10) override;

	struct EventDispatcherImpl;
	std::unique_ptr<EventDispatcherImpl> pimpl;
};

#endif // !__EVENT_DISPATCHER__
