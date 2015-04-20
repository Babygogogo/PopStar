#ifndef __SCRIPT__
#define __SCRIPT__

#include "../GameObject/Component.h"
#include "../Common/IUpdateable.h"

class Event;

class Script :public Component, public IUpdateable
{
	friend class EventDispatcher;

public:
	virtual ~Script(){};

protected:
	Script(const std::string &name, GameObject *game_object) :Component(name, game_object){};

	virtual void onEvent(Event *e) = 0;
};

#endif // !__SCRIPT__
