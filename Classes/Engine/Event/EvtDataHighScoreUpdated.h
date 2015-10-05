#ifndef __EVENT_DATA_HIGH_SCORE_UPDATED__
#define __EVENT_DATA_HIGH_SCORE_UPDATED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataHighScoreUpdated final : public BaseEventData
{
public:
	EvtDataHighScoreUpdated(int highScore) : m_HighScore{ highScore }
	{
	}

	virtual ~EvtDataHighScoreUpdated() = default;

	static const EventType EvtType = EventType::HighScoreUpdated;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getHighScore() const
	{
		return m_HighScore;
	}

	//Disable copy/move constructor and operator=.
	EvtDataHighScoreUpdated(const EvtDataHighScoreUpdated &) = delete;
	EvtDataHighScoreUpdated(EvtDataHighScoreUpdated &&) = delete;
	EvtDataHighScoreUpdated & operator=(const EvtDataHighScoreUpdated &) = delete;
	EvtDataHighScoreUpdated & operator=(EvtDataHighScoreUpdated &&) = delete;

private:
	int m_HighScore{ 0 };
};

#endif // !__EVENT_DATA_HIGH_SCORE_UPDATED__
