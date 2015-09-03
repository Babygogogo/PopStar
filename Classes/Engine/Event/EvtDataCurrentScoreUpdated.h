#ifndef __EVENT_DATA_CURRENT_SCORE_VALUE_UPDATED__
#define __EVENT_DATA_CURRENT_SCORE_VALUE_UPDATED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataCurrentScoreUpdated final : public BaseEventData
{
public:
	EvtDataCurrentScoreUpdated(int newCurrentScore) : m_NewCurrentScore{ newCurrentScore }
	{
	}

	virtual ~EvtDataCurrentScoreUpdated() = default;

	static const EventType EvtType = EventType::CurrentScoreUpdated;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getNewCurrentScore() const
	{
		return m_NewCurrentScore;
	}

	//Disable copy/move constructor and operator=.
	EvtDataCurrentScoreUpdated(const EvtDataCurrentScoreUpdated &) = delete;
	EvtDataCurrentScoreUpdated(EvtDataCurrentScoreUpdated &&) = delete;
	EvtDataCurrentScoreUpdated & operator=(const EvtDataCurrentScoreUpdated &) = delete;
	EvtDataCurrentScoreUpdated & operator=(EvtDataCurrentScoreUpdated &&) = delete;

private:
	int m_NewCurrentScore{ 0 };
};

#endif // !__EVENT_DATA_CURRENT_SCORE_VALUE_UPDATED__
