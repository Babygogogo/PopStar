#ifndef __EVENT_DATA_PLAYER_GOT_SCORE__
#define __EVENT_DATA_PLAYER_GOT_SCORE__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataPlayerGotScore final : public BaseEventData
{
public:
	EvtDataPlayerGotScore(int score, int explodedStarsCount) : m_Score{ score }, m_ExplodedStarsCount{ explodedStarsCount }
	{
	}

	virtual ~EvtDataPlayerGotScore() = default;

	static const EventType EvtType = EventType::PlayerGotScore;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getScore() const
	{
		return m_Score;
	}

	int getExplodedStarsCount() const
	{
		return m_ExplodedStarsCount;
	}

	//Disable copy/move constructor and operator=.
	EvtDataPlayerGotScore(const EvtDataPlayerGotScore &) = delete;
	EvtDataPlayerGotScore(EvtDataPlayerGotScore &&) = delete;
	EvtDataPlayerGotScore & operator=(const EvtDataPlayerGotScore &) = delete;
	EvtDataPlayerGotScore & operator=(EvtDataPlayerGotScore &&) = delete;

private:
	int m_Score{ 0 };
	int m_ExplodedStarsCount{ 0 };
};

#endif // !__EVENT_DATA_PLAYER_GOT_SCORE__
