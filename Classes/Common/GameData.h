#ifndef __GAME_DATA__
#define __GAME_DATA__

#include "Object.h"

#include <memory>

class GameData final : public Object
{
public:
	~GameData();

	static std::unique_ptr<GameData> create();

	void levelEnd();
	int getCurrentLevel() const;
	
	void updateCurrentScoreWith(int num_of_exploded_stars);
	int getScoreOfPreviousExplosion() const;
	int getExplodedStarsNum() const;

	int getHighScore() const;
	int getCurrentScore() const;
	int getTargetScore() const;

	int getStarsLeftNum() const;
	void setStarsLeftNum(int stars_left_num);
	void updateScoreWithEndLevelBonus();
	int getEndLevelBonus() const;

	void reset();

	//Disable copy/move constructor and operator=.
	GameData(const GameData&) = delete;
	GameData(GameData&&) = delete;
	GameData& operator=(const GameData&) = delete;
	GameData& operator=(GameData&&) = delete;

private:
	GameData();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif
