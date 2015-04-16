#ifndef _MENULAYER_H_
#define _MENULAYER_H_
#include "cocos2d.h"

class MenuLayer : public cocos2d::Layer{
public:
	virtual bool init();
	CREATE_FUNC(MenuLayer);
	void startGame();
};
#endif