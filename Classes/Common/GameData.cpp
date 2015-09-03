#include <array>

#include "cocos2d.h"

#include "GameData.h"
#include "../Engine/Event/EventDispatcher.h"
#include "../Engine/Event/EventType.h"
#include "../Engine/Event/BaseEventData.h"
#include "../Engine/Event/EvtDataGeneric.h"
#include "../Engine/Event/EvtDataCurrentScoreUpdated.h"
#include "../Engine/Event/EvtDataHighScoreUpdated.h"
#include "../Engine/Event/EvtDataLevelIndexUpdated.h"
#include "../Engine/Event/EvtDataLevelSummaryStarted.h"
#include "../Engine/Event/EvtDataPlayerGotScore.h"
#include "../Engine/Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameDataImpl.
//////////////////////////////////////////////////////////////////////////
struct GameData::GameDataImpl
{
	GameDataImpl();
	~GameDataImpl();

	void onNewGameStarted(const IEventData & e);
	void onLevelNoMoreMove(const IEventData & e);
	void onLevelSummaryFinished(const IEventData & e);

	void setCurrentScore(int score);
	void setHighScore(int score);
	void saveHighScore();
	int getScoreOf(int num_of_exploded_stars);
	int getEndLevelBonus() const;

	void setCurrentLevel(int level);
	void updateTargetScoreByCurrentLevel();

	int m_current_level{ 0 };
	int m_high_score{ 0 };
	int m_current_score{ 0 };
	int m_target_score{ 0 };
	int m_PreviousExplodedStarsCount{ 0 };
	int m_stars_left_num{ 0 };
};

GameData::GameDataImpl::GameDataImpl()
{
}

GameData::GameDataImpl::~GameDataImpl()
{
}

void GameData::GameDataImpl::onNewGameStarted(const IEventData & e)
{
	setCurrentLevel(1);
	setCurrentScore(0);
	setHighScore(cocos2d::UserDefault::getInstance()->getIntegerForKey("highestScore", 0));
}

void GameData::GameDataImpl::onLevelNoMoreMove(const IEventData & e)
{
	auto levelSummaryEvent = std::make_unique<EvtDataLevelSummaryStarted>(m_stars_left_num, getEndLevelBonus());
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(levelSummaryEvent));
}

void GameData::GameDataImpl::onLevelSummaryFinished(const IEventData & e)
{
	setCurrentScore(m_current_score + getEndLevelBonus());
}

void GameData::GameDataImpl::setCurrentScore(int score)
{
	if (score != m_current_score){
		m_current_score = score;
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataCurrentScoreUpdated>(m_current_score));

		setHighScore(m_current_score);
	}
}

void GameData::GameDataImpl::setHighScore(int score)
{
	if (m_high_score < score){
		m_high_score = score;
		saveHighScore();

		auto highScoreEvent = std::make_unique<EvtDataHighScoreUpdated>(score);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(highScoreEvent));
	}
}

void GameData::GameDataImpl::saveHighScore()
{
	cocos2d::UserDefault::getInstance()->setIntegerForKey("highestScore", m_high_score);
}

int GameData::GameDataImpl::getScoreOf(int num_of_exploded_stars)
{
	if (num_of_exploded_stars <= 1)
		throw("getScoreOf(exploded_stars_num) with a num <= 1 in GameData.");

	return num_of_exploded_stars * num_of_exploded_stars * 5;
}

void GameData::GameDataImpl::setCurrentLevel(int level)
{
	if (level < 0)
		throw("setCurrentLevel with a negative num in GameData.");

	m_current_level = level;
	m_PreviousExplodedStarsCount = 0;

	auto levelIndexEvent = std::make_unique<EvtDataLevelIndexUpdated>(level);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(levelIndexEvent));

	updateTargetScoreByCurrentLevel();
}

void GameData::GameDataImpl::updateTargetScoreByCurrentLevel()
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

	if (score != m_target_score){
		m_target_score = score;
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::TargetScoreValueUpdated));
	}
}

int GameData::GameDataImpl::getEndLevelBonus() const
{
	static const std::array<int, 10> bonus_array = { 2000, 1660, 1360, 1100, 880, 700, 560, 460, 400, 380 };

	if (m_stars_left_num > 9 || m_stars_left_num < 0)
		return 0;

	return bonus_array[m_stars_left_num];
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameData.
//////////////////////////////////////////////////////////////////////////
GameData::GameData() : pimpl{ std::make_unique<GameDataImpl>() }
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();

	eventDispatcher->vAddListener(EventType::NewGameStarted, pimpl, [this](const IEventData & e){
		pimpl->onNewGameStarted(e);
	});
	eventDispatcher->vAddListener(EventType::LevelNoMoreMove, pimpl, [this](const IEventData & e){
		pimpl->onLevelNoMoreMove(e);
	});
	eventDispatcher->vAddListener(EventType::LevelSummaryFinished, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryFinished(e);
	});
}

GameData::~GameData()
{
}

int GameData::getEndLevelBonus() const
{
	return pimpl->getEndLevelBonus();
}

void GameData::levelEnd()
{
	if (pimpl->m_current_score < pimpl->m_target_score)
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::GameOver));
	else{
		pimpl->setCurrentLevel(pimpl->m_current_level + 1);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::LevelStarted));
	}
}

int GameData::getCurrentLevel() const
{
	return pimpl->m_current_level;
}

int GameData::getTargetScore() const
{
	return pimpl->m_target_score;
}

void GameData::updateCurrentScoreWith(int num_of_exploded_stars)
{
	if (num_of_exploded_stars <= 0)
		return;

	pimpl->m_PreviousExplodedStarsCount = num_of_exploded_stars;
	auto gotScore = pimpl->getScoreOf(num_of_exploded_stars);
	pimpl->setCurrentScore(pimpl->m_current_score + gotScore);

	auto playerGotScoreEvent = std::make_unique<EvtDataPlayerGotScore>(gotScore, num_of_exploded_stars);
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::move(playerGotScoreEvent));
}

int GameData::getStarsLeftNum() const
{
	return pimpl->m_stars_left_num;
}

void GameData::setStarsLeftNum(int stars_left_num)
{
	pimpl->m_stars_left_num = stars_left_num;
}

void GameData::updateScoreWithEndLevelBonus()
{
	pimpl->setCurrentScore(pimpl->m_current_score + getEndLevelBonus());
}

int GameData::getExplodedStarsNum() const
{
	return pimpl->m_PreviousExplodedStarsCount;
}

int GameData::getScoreOfPreviousExplosion() const
{
	return pimpl->getScoreOf(pimpl->m_PreviousExplodedStarsCount);
}

int GameData::getHighScore() const
{
	return pimpl->m_high_score;
}

int GameData::getCurrentScore() const
{
	return pimpl->m_current_score;
}

std::unique_ptr<GameData> GameData::create()
{
	return std::unique_ptr<GameData>(new GameData);
}
