#ifndef __EVENT_TYPE__
#define __EVENT_TYPE__

enum class EventType
{
	InvalidEventType,

	RequestDestoryActor,

	PlayerGotScore,

	PlayerExplodedStars,

	LevelNoMoreMove,

	NewGameStarted,
	LevelStarted,
	StartLevelLabelDisappeared,
	LevelSummaryStarted,
	LevelSummaryFinished,

	GameOver,

	CurrentScoreUpdated,
	HighScoreUpdated,
	TargetScoreValueUpdated,
	LevelIndexUpdated
};

#endif // !__EVENT_TYPE__
