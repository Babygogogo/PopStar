#ifndef _STARMATRIX_H_
#define _STARMATRIX_H_
#include "cocos2d.h"
#include "Star.h"
#include <deque>

class StarMatrix : public cocos2d::Node
{
public:
	//static StarMatrix* create(GameLayer* layer);
	//virtual bool init(GameLayer* layer);
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
	//GameLayer* m_layer;
	bool needClear;
	static float ONE_CLEAR_TIME;
	float clearSumTime;

public:
	static StarMatrix *create(std::function<void()> &&layerHideLinkNum, std::function<void(int)> &&layerShowLinkNum,
		std::function<void(int)> &&layerFloatLeftStarMsg, std::function<void()> &&layerRefreshMenu,
		std::function<void()> &&layerGotoNextLevel, std::function<void()> &&layerGotoGameOver);

	bool init(std::function<void()> &&layerHideLinkNum, std::function<void(int)> &&layerShowLinkNum,
		std::function<void(int)> &&layerFloatLeftStarMsg, std::function<void()> &&layerRefreshMenu,
		std::function<void()> &&layerGotoNextLevel, std::function<void()> &&layerGotoGameOver);

	std::function<void()> m_layerHideLinkNum;
	std::function<void(int)> m_layerShowLinkNum;
	std::function<void(int)> m_layerFloatLeftStarMsg;
	std::function<void()> m_layerRefreshMenu;
	std::function<void()> m_layerGotoNextLevel;
	std::function<void()> m_layerGotoGameOver;
};

#endif
