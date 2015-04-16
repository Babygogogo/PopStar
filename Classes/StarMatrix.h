#ifndef _STARMATRIX_H_
#define _STARMATRIX_H_
#include "cocos2d.h"
#include "Star.h"
#include "GameLayer.h"
#include <deque>

class GameLayer;
class StarMatrix : public cocos2d::Node{
public:
	static StarMatrix* create(GameLayer* layer);
	virtual bool init(GameLayer* layer);
	void updateStar(float delta);
	void onTouch(const cocos2d::Point& p);
	void setNeedClear(bool b);
	 void initMatrix();
	
private:
   
	cocos2d::Point getPositionByIndex(int i, int j);
	Star* getStarByTouch(const cocos2d::Point& p);
	void genSelectedList(Star* s);
	void deleteSelectedList();
	void adjustMatrix();
	void refreshScore();
	bool isEnded();
	void clearMatrixOneByOne();
	int getLeftStarNum();
public:
	const static int ROW_NUM = 10;
	const static int COL_NUM = 10;
private:
	Star* stars[ROW_NUM][COL_NUM];
	std::deque<Star*> selectedList;
	GameLayer* m_layer;
	bool needClear;
	static float ONE_CLEAR_TIME;
	float clearSumTime;
};
#endif