#include "GameData.h"
#include "cocos2d.h"
#include "SingletonContainer.h"
#include "./Event/EventDispatcher.h"
#include "./Event/EventType.h"
#include "./Event/Event.h"

struct GameData::impl
{
	impl(){};
	~impl(){};

	void setCurrentScore(int score);
	void setHighScore(int score);
	void saveHighScore();
	int getScoreOf(int num_of_exploded_stars);
	
	void setCurrentLevel(int level);
	void updateTargetScoreByLevel();

	int current_level{ 0 };
	int high_score{ 0 };
	int current_score{ 0 };
	int target_score{ 0 };
	int num_of_previous_exploded_stars{ 0 };
	int stars_left_num{ 0 };
};

void GameData::impl::setCurrentScore(int score)
{
	if (score != current_score){
		current_score = score;
		SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::CurrentScoreValueUpdated));

		setHighScore(current_score);
	}
}

void GameData::impl::setHighScore(int score)
{
	if (high_score < score){
		high_score = score;
		saveHighScore();

		SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::HighScoreValueUpdated));
	}
}

void GameData::impl::saveHighScore()
{
	cocos2d::UserDefault::getInstance()->setIntegerForKey("highestScore", high_score);
}

int GameData::impl::getScoreOf(int num_of_exploded_stars)
{
	if (num_of_exploded_stars <= 1)
		return 0;

	return num_of_exploded_stars * num_of_exploded_stars * 5;
}

void GameData::impl::setCurrentLevel(int level)
{
	if (level < 0){
		return;
	}

	current_level = level;
	num_of_previous_exploded_stars = 0;

	SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::LevelValueUpdated));

	updateTargetScoreByLevel();
}

void GameData::impl::updateTargetScoreByLevel()
{
	int score = 0;
	if (current_level == 1){
		score = 1000;
	}
	else if (current_level == 2){
		score = 3000;
	}
	else if ((current_level >= 3) && (current_level <= 10)){
		score = 3000 + 3000 * (current_level - 2);
	}
	else{
		score = 27000 + 4000 * (current_level - 10);
	}

	if (score != target_score){
		target_score = score;
		SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::TargetScoreValueUpdated));
	}
}

GameData::GameData() :Object("GameData"), pimpl(new impl)
{
	reset();
}

int GameData::getEndLevelBonus() const
{
	static const int bonus_array[10][2] =
	{
		{0, 2000},
		{1, 1980},
		{2, 1920},
		{3, 1820},
		{4, 1680},
		{5, 1500},
		{6, 1280},
		{7, 1020},
		{8, 720},
		{9, 380}
	};
	if (pimpl->stars_left_num>9 || pimpl->stars_left_num<0){
		return 0;
	}
	return bonus_array[pimpl->stars_left_num][1];
}

void GameData::levelEnd()
{
	if (pimpl->current_score < pimpl->target_score)
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::GameOver));
	else{
		pimpl->setCurrentLevel(pimpl->current_level + 1);
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelStarted));
	}
}

int GameData::getCurrentLevel() const
{
	return pimpl->current_level;
}

int GameData::getTargetScore() const
{
	return pimpl->target_score;
}

void GameData::updateCurrentScoreWith(int num_of_exploded_stars)
{
	if (num_of_exploded_stars <= 0)
		return;

	pimpl->num_of_previous_exploded_stars = num_of_exploded_stars;
	pimpl->setCurrentScore(pimpl->current_score + pimpl->getScoreOf(num_of_exploded_stars));

	SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::CurrentScoreIncreased));
}

int GameData::getStarsLeftNum() const
{
	return pimpl->stars_left_num;
}

void GameData::setStarsLeftNum(int stars_left_num)
{
	pimpl->stars_left_num = stars_left_num;
}

void GameData::updateScoreWithEndLevelBonus()
{
	pimpl->setCurrentScore(pimpl->current_score + getEndLevelBonus());
}

int GameData::getExplodedStarsNum() const
{
	return pimpl->num_of_previous_exploded_stars;
}

int GameData::getScoreOfPreviousExplosion() const
{
	return pimpl->getScoreOf(pimpl->num_of_previous_exploded_stars);
}

int GameData::getHighScore() const
{
	return pimpl->high_score;
}

int GameData::getCurrentScore() const
{
	return pimpl->current_score;
}

std::unique_ptr<GameData> GameData::create()
{
	return std::unique_ptr<GameData>(new GameData);
}

void GameData::reset()
{
	pimpl->setCurrentLevel(1);
	pimpl->setCurrentScore(0);
	pimpl->setHighScore(cocos2d::UserDefault::getInstance()->getIntegerForKey("highestScore", 0));
}

GameData::~GameData()
{

}
