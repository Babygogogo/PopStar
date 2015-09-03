#ifndef __EVENT_DATA_TARGET_SCORE_UPDATED__
#define __EVENT_DATA_TARGET_SCORE_UPDATED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataTargetScoreUpdated final : public BaseEventData
{
public:
	EvtDataTargetScoreUpdated(int targetScore) : m_TargetScore{ targetScore }
	{
	}

	virtual ~EvtDataTargetScoreUpdated() = default;

	static const EventType EvtType = EventType::TargetScoreUpdated;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getTargetScore() const
	{
		return m_TargetScore;
	}

	//Disable copy/move constructor and operator=.
	EvtDataTargetScoreUpdated(const EvtDataTargetScoreUpdated &) = delete;
	EvtDataTargetScoreUpdated(EvtDataTargetScoreUpdated &&) = delete;
	EvtDataTargetScoreUpdated & operator=(const EvtDataTargetScoreUpdated &) = delete;
	EvtDataTargetScoreUpdated & operator=(EvtDataTargetScoreUpdated &&) = delete;

private:
	int m_TargetScore{ 0 };
};

#endif // !__EVENT_DATA_TARGET_SCORE_UPDATED__
