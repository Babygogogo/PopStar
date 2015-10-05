#ifndef __EVENT_DATA_LEVEL_STARTED__
#define __EVENT_DATA_LEVEL_STARTED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataLevelStarted final : public BaseEventData
{
public:
	EvtDataLevelStarted(int levelIndex) : m_LevelIndex{ levelIndex }
	{
	}

	virtual ~EvtDataLevelStarted() = default;

	static const EventType EvtType = EventType::LevelStarted;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getLevelIndex() const
	{
		return m_LevelIndex;
	}

	//Disable copy/move constructor and operator=.
	EvtDataLevelStarted(const EvtDataLevelStarted &) = delete;
	EvtDataLevelStarted(EvtDataLevelStarted &&) = delete;
	EvtDataLevelStarted & operator=(const EvtDataLevelStarted &) = delete;
	EvtDataLevelStarted & operator=(EvtDataLevelStarted &&) = delete;

private:
	int m_LevelIndex{ 0 };
};

#endif // !__EVENT_DATA_LEVEL_STARTED__
