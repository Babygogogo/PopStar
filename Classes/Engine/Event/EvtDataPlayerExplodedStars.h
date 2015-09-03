#ifndef __EVENT_DATA_PLAYER_EXPLODED_STARS__
#define __EVENT_DATA_PLAYER_EXPLODED_STARS__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataPlayerExplodedStars final : public BaseEventData
{
public:
	EvtDataPlayerExplodedStars(int explodedStarsCount) : m_ExplodedStarsCount{ explodedStarsCount }
	{
	}

	virtual ~EvtDataPlayerExplodedStars() = default;

	static const EventType EvtType = EventType::PlayerExplodedStars;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getExplodedStarsCount() const
	{
		return m_ExplodedStarsCount;
	}

	//Disable copy/move constructor and operator=.
	EvtDataPlayerExplodedStars(const EvtDataPlayerExplodedStars &) = delete;
	EvtDataPlayerExplodedStars(EvtDataPlayerExplodedStars &&) = delete;
	EvtDataPlayerExplodedStars & operator=(const EvtDataPlayerExplodedStars &) = delete;
	EvtDataPlayerExplodedStars & operator=(EvtDataPlayerExplodedStars &&) = delete;

private:
	int m_ExplodedStarsCount{ 0 };
};

#endif // !__EVENT_DATA_PLAYER_EXPLODED_STARS__
