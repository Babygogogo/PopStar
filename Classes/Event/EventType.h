#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

enum class EventType
{
	StarsExploded,

	LevelResultBegan,
	LevelResultEnded,

	CurrentScoreValueUpdated,
	HighScoreValueUpdated,
	TargetScoreValueUpdated,
	LevelValueUpdated
};

#endif // !__EVENT_TYPE__
