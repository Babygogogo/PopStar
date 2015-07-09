#ifndef __EVENT_ARG_1__
#define __EVENT_ARG_1__

#include <memory>

#include "../../Common/Object.h"

class EventArg1 final :public Object
{
	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	EventArg1(const EventArg1&) = delete;
	EventArg1(EventArg1&&) = delete;
	EventArg1& operator=(const EventArg1&) = delete;
	EventArg1& operator=(EventArg1&&) = delete;
	
public:
	static std::unique_ptr<EventArg1> create(int arg)
	{
		return std::unique_ptr<EventArg1>(new EventArg1(arg));
	}

	~EventArg1(){};

	int getInt(){ return m_int; }

private:
	EventArg1(int arg) :Object("EventArg1"), m_int(arg){}

	int m_int{ 0 };
};

#endif // !__EVENT_ARG_1__
