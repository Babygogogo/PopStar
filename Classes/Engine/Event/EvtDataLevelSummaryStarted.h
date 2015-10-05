#ifndef __EVENT_DATA_LEVEL_SUMMARY_STARTED__
#define __EVENT_DATA_LEVEL_SUMMARY_STARTED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataLevelSummaryStarted final : public BaseEventData
{
public:
	EvtDataLevelSummaryStarted(int leftStarsCount, int bonusScore) : m_LeftStarsCount{ leftStarsCount }, m_BonusScore{ bonusScore }
	{
	}

	virtual ~EvtDataLevelSummaryStarted() = default;

	static const EventType EvtType = EventType::LevelSummaryStarted;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getLeftStarsCount() const
	{
		return m_LeftStarsCount;
	}

	int getBonusScore() const
	{
		return m_BonusScore;
	}

	//Disable copy/move constructor and operator=.
	EvtDataLevelSummaryStarted(const EvtDataLevelSummaryStarted &) = delete;
	EvtDataLevelSummaryStarted(EvtDataLevelSummaryStarted &&) = delete;
	EvtDataLevelSummaryStarted & operator=(const EvtDataLevelSummaryStarted &) = delete;
	EvtDataLevelSummaryStarted & operator=(EvtDataLevelSummaryStarted &&) = delete;

private:
	int m_LeftStarsCount{ 0 };
	int m_BonusScore{ 0 };
};

#endif // !__EVENT_DATA_LEVEL_SUMMARY_STARTED__
