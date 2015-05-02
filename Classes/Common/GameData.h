#ifndef __GAME_DATA__
#define __GAME_DATA__

#include "Object.h"

#include <memory>

class GameData final : public Object
{
	friend class SingletonContainer;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	GameData(const GameData&) = delete;
	GameData(GameData&&) = delete;
	GameData& operator=(const GameData&) = delete;
	GameData& operator=(GameData&&) = delete;
	
public:
	~GameData();

	void levelEnd();
	int getCurrentLevel() const;
	
	void updateCurrentScoreWith(int num_of_exploded_stars);
	int getScoreOfPreviousExplosion() const;
	int getExplodedStarsNum() const;

	int getHighScore() const;
	int getCurrentScore() const;
	int getTargetScore() const;

	void updateEndLevelScoreWith(int num_of_left_stars);
	int getEndLevelBonus(int num_of_left_stars) const;

	void reset();

private:
	static std::unique_ptr<GameData> create();
	GameData();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif
