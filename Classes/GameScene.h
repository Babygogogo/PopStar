#ifndef _GAMESCENE_H_
#define _GAMESCENE_H_
#include "cocos2d.h"

class GameScene : public cocos2d::Scene{
public:
	virtual bool init();
	CREATE_FUNC(GameScene);
};
#endif