#ifndef __LEGACY_EVENT__
#define __LEGACY_EVENT__

#include <memory>

#include "../../Common/Object.h"

enum class LegacyEventType;

class LegacyEvent : public Object
{
	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	LegacyEvent(const LegacyEvent&) = delete;
	LegacyEvent(LegacyEvent&&) = delete;
	LegacyEvent& operator=(const LegacyEvent&) = delete;
	LegacyEvent& operator=(LegacyEvent&&) = delete;
	
public:
	static std::unique_ptr<LegacyEvent> create(LegacyEventType event_type, std::unique_ptr<Object> &&event_arg = nullptr)
	{
		return std::unique_ptr<LegacyEvent>(new LegacyEvent("Event", event_type, std::move(event_arg)));
	}

	LegacyEventType getType() const{ return m_event_type; }
	Object *getArg() const
	{
		return m_event_arg ? m_event_arg.get() : nullptr;
	}

protected:
	LegacyEvent(std::string &&name, LegacyEventType event_type, std::unique_ptr<Object> &&event_arg)
		:Object(std::move(name)), m_event_type(std::move(event_type)), m_event_arg(std::move(event_arg)){};

private:
	LegacyEventType m_event_type;
	std::unique_ptr<Object> m_event_arg;
};

#endif // !__LEGACY_EVENT__
