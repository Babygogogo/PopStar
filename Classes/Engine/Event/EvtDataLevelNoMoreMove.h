#ifndef __EVENT_DATA_LEVEL_NO_MORE_MOVE__
#define __EVENT_DATA_LEVEL_NO_MORE_MOVE__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataLevelNoMoreMove final : public BaseEventData
{
public:
	EvtDataLevelNoMoreMove(int leftStarsCount) : m_LeftStarsCount{ leftStarsCount }
	{
	}

	virtual ~EvtDataLevelNoMoreMove() = default;

	static const EventType EvtType = EventType::LevelNoMoreMove;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getLeftStarsCount() const
	{
		return m_LeftStarsCount;
	}

	//Disable copy/move constructor and operator=.
	EvtDataLevelNoMoreMove(const EvtDataLevelNoMoreMove &) = delete;
	EvtDataLevelNoMoreMove(EvtDataLevelNoMoreMove &&) = delete;
	EvtDataLevelNoMoreMove & operator=(const EvtDataLevelNoMoreMove &) = delete;
	EvtDataLevelNoMoreMove & operator=(EvtDataLevelNoMoreMove &&) = delete;

private:
	int m_LeftStarsCount{ 0 };
};

#endif // !__EVENT_DATA_LEVEL_NO_MORE_MOVE__
