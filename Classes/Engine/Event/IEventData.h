#ifndef __I_EVENT_DATA__
#define __I_EVENT_DATA__

#include "EventType.h"

/*!
 * \class IEventData
 *
 * \brief The interface of the EventData.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class IEventData
{
public:
	virtual const EventType & getEventType() const = 0;
};

#endif // !__I_EVENT_DATA__
