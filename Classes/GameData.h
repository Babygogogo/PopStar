#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

class GAMEDATA{
public:
	static GAMEDATA* getInstance();

	void levelUp();
	int getCurrentLevel();

	inline int getHistoryScore(){return history_score;}

	/*当前分数*/
	inline void setCurrentScore(int score);
	inline int getCurScore(){return cur_score;}

	/*通关分数*/
	int getTargetScore();

	/*奖励分数*/
	int getJiangli(int size);

	void saveHighestScore();

	void init();

private:
	GAMEDATA();

private:
	void setHighScore(int score);

	void setCurrentLevel(int level);
	void updateTargetScoreByLevel();

	static GAMEDATA* _instance;
	int current_level;
	int history_score;
	int cur_score;
	int target_score;
};

#endif
