#ifndef _LEVELMSGLAYER_H_
#define _LEVELMSGLAYER_H_
#include "cocos2d.h"

class LevelMsgLayer : public cocos2d::Layer{
public:
	virtual bool init();
	CREATE_FUNC(LevelMsgLayer);
private:
	virtual void update(float delta);
};
#endif