#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

enum class EventType
{
	SequentialInvokerFinishOneAction,

	StarsExploded,

	LevelResultBegan,
	LevelResultEnded,
	LevelUp,
	LevelNoMoreMove,

	GameOver,

	CurrentScoreValueUpdated,
	HighScoreValueUpdated,
	TargetScoreValueUpdated,
	LevelValueUpdated
};

#endif // !__EVENT_TYPE__
