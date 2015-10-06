#include <array>
#include <cassert>
#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "ScoreCalculatorScript.h"
#include "../Event/BaseEventData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataCurrentScoreUpdated.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataHighScoreUpdated.h"
#include "../Event/EvtDataLevelNoMoreMove.h"
#include "../Event/EvtDataLevelStarted.h"
#include "../Event/EvtDataLevelSummaryStarted.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "../Event/EvtDataPlayerGotScore.h"
#include "../Event/EvtDataTargetScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"
#include "../Utilities/StringToVector.h"

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
	void onRemainingStarsExploded(const IEventData & e);

	int getExplodeScore(int explodedStarsCount) const;
	int getEndLevelBonus() const;
	int getTargetScore(int levelIndex) const;

	void updateCurrentAndHighScore(int score);
	void updateCurrentLevelAndTargetScore(int level);

	static std::vector<int> s_EndLevelBonusArray;
	static std::vector<int> s_TargetScoreArray;
	static int s_TargetScoreIncrement;

	int m_CurrentLevel{ 0 };
	int m_HighScore{ -1 };
	int m_CurrentScore{ 0 };
	int m_TargetScore{ 0 };
	int m_RemainingStarsCount{ 0 };
};

std::vector<int> ScoreCalculatorScript::ScoreCalculatorScriptImpl::s_EndLevelBonusArray;
std::vector<int> ScoreCalculatorScript::ScoreCalculatorScriptImpl::s_TargetScoreArray;
int ScoreCalculatorScript::ScoreCalculatorScriptImpl::s_TargetScoreIncrement;

ScoreCalculatorScript::ScoreCalculatorScriptImpl::ScoreCalculatorScriptImpl()
{
}

ScoreCalculatorScript::ScoreCalculatorScriptImpl::~ScoreCalculatorScriptImpl()
{
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onNewGameStarted(const IEventData & e)
{
	updateCurrentAndHighScore(0);
	updateCurrentLevelAndTargetScore(1);
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onPlayerExplodedStars(const IEventData & e)
{
	const auto & playerExplodedStarsEvent = static_cast<const EvtDataPlayerExplodedStars &>(e);
	auto explodedStarsCount = playerExplodedStarsEvent.getExplodedStarsCount();

	auto explodeScore = getExplodeScore(explodedStarsCount);
	updateCurrentAndHighScore(m_CurrentScore + explodeScore);

	auto playerGotScoreEvent = std::make_unique<EvtDataPlayerGotScore>(explodeScore, explodedStarsCount);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(playerGotScoreEvent));
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onLevelNoMoreMove(const IEventData & e)
{
	const auto & noMoreMoveEvent = static_cast<const EvtDataLevelNoMoreMove &>(e);
	m_RemainingStarsCount = noMoreMoveEvent.getRemainingStarsCount();

	auto levelSummaryEvent = std::make_unique<EvtDataLevelSummaryStarted>(m_RemainingStarsCount, getEndLevelBonus());
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(levelSummaryEvent));
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onLevelSummaryFinished(const IEventData & e)
{
	updateCurrentAndHighScore(m_CurrentScore + getEndLevelBonus());
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::onRemainingStarsExploded(const IEventData & e)
{
	if (m_CurrentScore < m_TargetScore)
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::GameOver));
	else
		updateCurrentLevelAndTargetScore(m_CurrentLevel + 1);
}

int ScoreCalculatorScript::ScoreCalculatorScriptImpl::getExplodeScore(int explodedStarsCount) const
{
	assert(explodedStarsCount > 1 && "ScoreCalculatorScriptImpl::getExplodeScore() with exploded count <= 1.");

	return explodedStarsCount * explodedStarsCount * 5;
}

int ScoreCalculatorScript::ScoreCalculatorScriptImpl::getEndLevelBonus() const
{
	if (m_RemainingStarsCount >= s_EndLevelBonusArray.size() || m_RemainingStarsCount < 0)
		return 0;

	return s_EndLevelBonusArray[m_RemainingStarsCount];
}

int ScoreCalculatorScript::ScoreCalculatorScriptImpl::getTargetScore(int levelIndex) const
{
	assert(levelIndex > 0 && "ScoreCalculatorScriptImpl::getTargetScore() with a non-positive level.");
	if (levelIndex <= s_TargetScoreArray.size())
		return s_TargetScoreArray[levelIndex - 1];
	else
		return s_TargetScoreArray.back() + (levelIndex - s_TargetScoreArray.size()) * s_TargetScoreIncrement;
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::updateCurrentAndHighScore(int score)
{
	//Update current score and dispatch an event.
	if (score != m_CurrentScore){
		m_CurrentScore = score;
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataCurrentScoreUpdated>(m_CurrentScore));
	}

	//Update high score if needed.
	auto newHighScore = score;
	if (newHighScore == 0)
		newHighScore = cocos2d::UserDefault::getInstance()->getIntegerForKey("highestScore", 0);
	if (newHighScore > m_HighScore){
		m_HighScore = newHighScore;
		cocos2d::UserDefault::getInstance()->setIntegerForKey("highestScore", m_HighScore);

		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataHighScoreUpdated>(m_HighScore));
	}
}

void ScoreCalculatorScript::ScoreCalculatorScriptImpl::updateCurrentLevelAndTargetScore(int level)
{
	assert(level > 0 && "ScoreCalculatorScriptImpl::setCurrentLevel() with a non-positive level.");

	m_CurrentLevel = level;
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataLevelStarted>(level));

	//Update target score by current level.
	auto score = getTargetScore(m_CurrentLevel);
	if (score != m_TargetScore){
		m_TargetScore = score;
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataTargetScoreUpdated>(m_TargetScore));
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameData.
//////////////////////////////////////////////////////////////////////////
ScoreCalculatorScript::ScoreCalculatorScript() : pimpl{ std::make_shared<ScoreCalculatorScriptImpl>() }
{
}

ScoreCalculatorScript::~ScoreCalculatorScript()
{
}

bool ScoreCalculatorScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	//Load the end level bonus array.
	pimpl->s_EndLevelBonusArray = toVector<int>(xmlElement->FirstChildElement("EndLevelBonus")->Attribute("Array"));

	//Load the target score array.
	auto targetScoreElement = xmlElement->FirstChildElement("TargetScore");
	pimpl->s_TargetScoreArray = toVector<int>(targetScoreElement->Attribute("Array"));
	pimpl->s_TargetScoreIncrement = targetScoreElement->IntAttribute("Increment");

	isStaticInitialized = true;
	return true;
}

void ScoreCalculatorScript::vPostInit()
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
	eventDispatcher->vAddListener(EventType::RemainingStarsExploded, pimpl, [this](const IEventData & e){
		pimpl->onRemainingStarsExploded(e);
	});
}

const std::string & ScoreCalculatorScript::getType() const
{
	return Type;
}

const std::string ScoreCalculatorScript::Type = "ScoreCalculatorScript";
