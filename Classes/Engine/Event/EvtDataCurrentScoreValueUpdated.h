#ifndef __EVENT_DATA_CURRENT_SCORE_VALUE_UPDATED__
#define __EVENT_DATA_CURRENT_SCORE_VALUE_UPDATED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataCurrentScoreValueUpdated : public BaseEventData
{
public:
	EvtDataCurrentScoreValueUpdated(int newCurrentScore) : m_NewCurrentScore{ newCurrentScore }
	{
	}

	virtual ~EvtDataCurrentScoreValueUpdated() = default;

	static const EventType EvtType = EventType::CurrentScoreValueUpdated;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getNewCurrentScore() const
	{
		return m_NewCurrentScore;
	}

private:
	int m_NewCurrentScore{ 0 };
};

#endif // !__EVENT_DATA_CURRENT_SCORE_VALUE_UPDATED__
