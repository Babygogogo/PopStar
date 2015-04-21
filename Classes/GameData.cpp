#include "GameData.h"
#include "cocos2d.h"
#include "./Common/SingletonContainer.h"
#include "./Event/EventDispatcher.h"
#include "./Event/EventType.h"
#include "./Event/Event.h"

GAMEDATA* GAMEDATA::_instance = 0;

GAMEDATA::GAMEDATA(){
	this->init();
}
GAMEDATA* GAMEDATA::getInstance(){
	if(_instance == 0){
		_instance = new GAMEDATA();
	}
	return _instance;
}

void GAMEDATA::init(){
	setCurrentLevel(1);
	setCurrentScore(0);
	setHighScore(cocos2d::UserDefault::getInstance()->getIntegerForKey("highestScore",0));
}

void GAMEDATA::setCurrentLevel(int level){
	if(level < 0){
		return;
	}

	current_level = level;
	SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::LevelValueUpdated));

	updateTargetScoreByLevel();
}

void GAMEDATA::updateTargetScoreByLevel()
{
	int score = 0;
	if (current_level == 1){
		score = 1000;
	}
	else if (current_level == 2){
		score = 3000;
	}
	else if ( (current_level >=3) && (current_level <= 10) ){
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

int GAMEDATA::getJiangli(int size){
	static const int jiangli[10][2] =
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
	if(size>9 || size<0){
		return 0;
	}
	return jiangli[size][1];
}


void GAMEDATA::saveHighestScore(){
	cocos2d::UserDefault::getInstance()->setIntegerForKey("highestScore",getHistoryScore());
}

void GAMEDATA::setHighScore(int score)
{
	if (history_score < score){
		history_score = score;
		SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::HighScoreValueUpdated));
	}
}

void GAMEDATA::setCurrentScore(int score)
{
	if (score != cur_score){
		cur_score = score;
		SingletonContainer::instance()->get<EventDispatcher>()->dispatch(Event::create(EventType::CurrentScoreValueUpdated));

		setHighScore(cur_score);
	}
}

void GAMEDATA::levelUp()
{
	setCurrentLevel(current_level + 1);
}

int GAMEDATA::getCurrentLevel()
{
	return current_level;
}

int GAMEDATA::getTargetScore()
{
	return target_score;
}
