#ifndef __I_EVENT_LISTENER__
#define __I_EVENT_LISTENER__

class Event;

class IEventListener
{
	friend class EventDispatcher;

protected:
	inline virtual void onEvent(Event* e) = 0;
};

#endif // !__I_EVENT_LISTENER__
