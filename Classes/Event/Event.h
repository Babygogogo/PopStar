#ifndef __EVENT__
#define __EVENT__

#include <memory>

#include "../Common/Object.h"
#include "EventType.h"

class Event : public Object
{
public:
	static std::unique_ptr<Event> create(EventType event_type, std::unique_ptr<Object> &&event_arg = nullptr){
		return std::unique_ptr<Event>(new Event("Event", event_type, std::move(event_arg)));
	}

	EventType getType() const{ return m_event_type; }
	Object *getArg() const
	{
		return m_event_arg ? m_event_arg.get() : nullptr;
	}

protected:
	Event(const std::string &name, EventType event_type, std::unique_ptr<Object> &&event_arg)
		:Object(name), m_event_type(std::move(event_type)), m_event_arg(std::move(event_arg)){};

private:
	EventType m_event_type;
	std::unique_ptr<Object> m_event_arg;
};

#endif // !__EVENT__
