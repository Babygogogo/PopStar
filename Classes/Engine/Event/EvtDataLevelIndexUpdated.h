#ifndef __EVENT_DATA_LEVEL_INDEX_UPDATED__
#define __EVENT_DATA_LEVEL_INDEX_UPDATED__

#include "BaseEventData.h"
#include "EventType.h"

class EvtDataLevelIndexUpdated final : public BaseEventData
{
public:
	EvtDataLevelIndexUpdated(int levelIndex) : m_LevelIndex{ levelIndex }
	{
	}

	virtual ~EvtDataLevelIndexUpdated() = default;

	static const EventType EvtType = EventType::LevelIndexUpdated;

	virtual const EventType & getType() const override
	{
		return EvtType;
	}

	int getLevelIndex() const
	{
		return m_LevelIndex;
	}

	//Disable copy/move constructor and operator=.
	EvtDataLevelIndexUpdated(const EvtDataLevelIndexUpdated &) = delete;
	EvtDataLevelIndexUpdated(EvtDataLevelIndexUpdated &&) = delete;
	EvtDataLevelIndexUpdated & operator=(const EvtDataLevelIndexUpdated &) = delete;
	EvtDataLevelIndexUpdated & operator=(EvtDataLevelIndexUpdated &&) = delete;

private:
	int m_LevelIndex{ 0 };
};

#endif // !__EVENT_DATA_LEVEL_INDEX_UPDATED__
