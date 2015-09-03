#include <array>

#include "cocos2d.h"

#include "ScoreCalculatorScript.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataCurrentScoreUpdated.h"
#include "../Event/EvtDataHighScoreUpdated.h"
#include "../Event/EvtDataLevelStarted.h"
#include "../Event/EvtDataLevelSummaryStarted.h"
#include "../Event/EvtDataLevelNoMoreMove.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "../Event/EvtDataPlayerGotScore.h"
#include "../Event/EvtDataTargetScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameDataImpl.
//////////////////////////////////////////////////////////////////////////
struct ScoreCalculatorScript::ScoreCalculatorScriptImpl
{
	ScoreCalculatorScriptImpl();
	~ScoreCalculatorScriptImpl();

	void onNewGameStarted(const IEventData & e);
	void onPlayerExplodedStars(const IEventData & e);
	void onLevelNoMoreMove(const IEventData & e);
	void onLevelSummaryFinished(const IEventData & e);
	void onLeftStarsExploded(const IEventData & e);

	void setCurrentScore(int score);
	void setHighScore(int score);
	void saveHighScore();
	int getScoreOf(int num_of_exploded_stars);
	int getEndLevelBonus() const;

	void setCurrentLevel(int level);
	void updateTargetScoreByCurrentLevel();

	int m_current_level{ 0 };
	int m_HighScore{ 0 };
	int m_CurrentScore{ 0 };
	int m_TargetScore{ 0 };
	int m_PreviousExplodedStarsCount{ 0 };
	int m_stars_left_num{ 0 };
};

ScoreCalculatorScript::ScoreCalculatorScriptImpl::ScoreCalculatorScriptImpl()
{
}

ScoreCalculatorScript::ScoreCalculatorScriptImpl::~ScoreCalculatorScriptImpl()
{
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onNewGameStarted(const IEventData & e)
{
	setCurrentScore(0);
	setHighScore(cocos2d::UserDefault::getInstance()->getIntegerForKey("highestScore", 0));
	setCurrentLevel(1);
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onPlayerExplodedStars(const IEventData & e)
{
	const auto & playerExplodedStarsEvent = static_cast<const EvtDataPlayerExplodedStars &>(e);
	auto explodedStarsCount = playerExplodedStarsEvent.getExplodedStarsCount();

	m_PreviousExplodedStarsCount = explodedStarsCount;
	auto gotScore = getScoreOf(explodedStarsCount);
	setCurrentScore(m_CurrentScore + gotScore);

	auto playerGotScoreEvent = std::make_unique<EvtDataPlayerGotScore>(gotScore, explodedStarsCount);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(playerGotScoreEvent));
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onLevelNoMoreMove(const IEventData & e)
{
	const auto & noMoreMoveEvent = static_cast<const EvtDataLevelNoMoreMove &>(e);
	m_stars_left_num = noMoreMoveEvent.getLeftStarsCount();

	auto levelSummaryEvent = std::make_unique<EvtDataLevelSummaryStarted>(m_stars_left_num, getEndLevelBonus());
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(levelSummaryEvent));
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onLevelSummaryFinished(const IEventData & e)
{
	setCurrentScore(m_CurrentScore + getEndLevelBonus());
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onLeftStarsExploded(const IEventData & e)
{
	if (m_CurrentScore < m_TargetScore)
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::GameOver));
	else
		setCurrentLevel(m_current_level + 1);
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::setCurrentScore(int score)
{
	if (score != m_CurrentScore){
		m_CurrentScore = score;
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataCurrentScoreUpdated>(m_CurrentScore));

		setHighScore(m_CurrentScore);
	}
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::setHighScore(int score)
{
	if (m_HighScore <= score){
		m_HighScore = score;
		saveHighScore();

		auto highScoreEvent = std::make_unique<EvtDataHighScoreUpdated>(m_HighScore);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(highScoreEvent));
	}
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::saveHighScore()
{
	cocos2d::UserDefault::getInstance()->setIntegerForKey("highestScore", m_HighScore);
}

int ScoreCalculatorScript::ScoreCalculatorScriptImpl::getScoreOf(int num_of_exploded_stars)
{
	if (num_of_exploded_stars <= 1)
		throw("getScoreOf(exploded_stars_num) with a num <= 1 in GameData.");

	return num_of_exploded_stars * num_of_exploded_stars * 5;
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::setCurrentLevel(int level)
{
	if (level < 0)
		throw("setCurrentLevel with a negative num in GameData.");

	m_current_level = level;
	m_PreviousExplodedStarsCount = 0;

	auto levelStartedEvent = std::make_unique<EvtDataLevelStarted>(level);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(levelStartedEvent));

	updateTargetScoreByCurrentLevel();
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::updateTargetScoreByCurrentLevel()
{
	if (m_current_level <= 0)
		throw("updateTargetScoreByCurrentLevel when current_level <= 0");

	auto score = 0;
	if (m_current_level == 1)
		score = 1000;
	else if (m_current_level == 2)
		score = 3000;
	else if ((m_current_level >= 3) && (m_current_level <= 10))
		score = 3000 + 3000 * (m_current_level - 2);
	else
		score = 27000 + 4000 * (m_current_level - 10);

	if (score != m_TargetScore){
		m_TargetScore = score;

		auto targetScoreEvent = std::make_unique<EvtDataTargetScoreUpdated>(m_TargetScore);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(targetScoreEvent));
	}
}

int ScoreCalculatorScript::ScoreCalculatorScriptImpl::getEndLevelBonus() const
{
	static const std::array<int, 10> bonus_array = { 2000, 1660, 1360, 1100, 880, 700, 560, 460, 400, 380 };

	if (m_stars_left_num > 9 || m_stars_left_num < 0)
		return 0;

	return bonus_array[m_stars_left_num];
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameData.
//////////////////////////////////////////////////////////////////////////
ScoreCalculatorScript::ScoreCalculatorScript() : pimpl{ std::make_unique<ScoreCalculatorScriptImpl>() }
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();

	eventDispatcher->vAddListener(EventType::NewGameStarted, pimpl, [this](const IEventData & e){
		pimpl->onNewGameStarted(e);
	});
	eventDispatcher->vAddListener(EventType::PlayerExplodedStars, pimpl, [this](const IEventData &e){
		pimpl->onPlayerExplodedStars(e);
	});
	eventDispatcher->vAddListener(EventType::LevelNoMoreMove, pimpl, [this](const IEventData & e){
		pimpl->onLevelNoMoreMove(e);
	});
	eventDispatcher->vAddListener(EventType::LevelSummaryFinished, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryFinished(e);
	});
	eventDispatcher->vAddListener(EventType::LeftStarsExploded, pimpl, [this](const IEventData & e){
		pimpl->onLeftStarsExploded(e);
	});
}

ScoreCalculatorScript::~ScoreCalculatorScript()
{
}

const std::string & ScoreCalculatorScript::getType() const
{
	return Type;
}

const std::string ScoreCalculatorScript::Type = "ScoreCalculatorScript";
