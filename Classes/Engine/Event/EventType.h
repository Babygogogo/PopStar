#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

enum class EventType
{
	InvalidEventType,

	RequestDestoryActor,

	SequentialInvokerFinishOneAction,

	CurrentScoreIncreased,

	PlayerExplodedStars,

	LevelNoMoreMove,

	LevelStarted,
	StartLevelLabelDisappeared,
	LevelSummaryLabelDisappeared,

	GameOver,

	CurrentScoreValueUpdated,
	HighScoreValueUpdated,
	TargetScoreValueUpdated,
	LevelValueUpdated
};

#endif // !__EVENT_TYPE__
