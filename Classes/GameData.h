#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

class GAMEDATA{
public:
	static GAMEDATA* getInstance();

	void levelUp();
	int getCurrentLevel();
	
	void updateCurrentScoreWith(int num_of_exploded_stars);
	int getScoreOfPreviousExplosion();
	int getNumExplodedStars();

	int getHighScore();
	int getCurrentScore();
	int getTargetScore();

	void updateEndLevelScoreWith(int num_of_left_stars);
	int getEndLevelBonus(int num_of_left_stars);

	void saveHighestScore();

	void init();

private:
	GAMEDATA();

private:
	void setHighScore(int score);
	void setCurrentScore(int score);

	void setCurrentLevel(int level);
	void updateTargetScoreByLevel();

	int getScoreOf(int num_of_exploded_stars);

	static GAMEDATA* _instance;

	int current_level;
	int high_score;
	int current_score;
	int target_score;
	int num_of_previous_exploded_stars{ 0 };
};

#endif
