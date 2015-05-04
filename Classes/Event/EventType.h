#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

enum class EventType
{
	SequentialInvokerFinishOneAction,

	CurrentScoreIncreased,

	LevelResultBegan,
	LevelResultEnded,
	LevelNoMoreMove,

	LevelStarted,
	StartLevelLabelDisappeared,

	GameOver,

	CurrentScoreValueUpdated,
	HighScoreValueUpdated,
	TargetScoreValueUpdated,
	LevelValueUpdated
};

#endif // !__EVENT_TYPE__
