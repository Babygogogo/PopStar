#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

using EventType = unsigned long;

enum class LegacyEventType
{
	SequentialInvokerFinishOneAction,

	CurrentScoreIncreased,

	UserClickedStarsExploded,

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
