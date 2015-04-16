#ifndef _TOPMENU_H_
#define _TOPMENU_H_
#include "cocos2d.h"

class TopMenu : public cocos2d::Node{
public:
	virtual bool init();
	CREATE_FUNC(TopMenu);
	void refresh();
private:
	cocos2d::Label* highestScore;
	cocos2d::Label* level;
	cocos2d::Label* targetScore;
	cocos2d::Label* curScore;
};
#endif