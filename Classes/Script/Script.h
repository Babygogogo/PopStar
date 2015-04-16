#ifndef __SCRIPT__
#define __SCRIPT__

#include "../GameObject/Component.h"
#include "../Common/IUpdateable.h"

class Script :public Component, public IUpdateable
{
public:
	virtual ~Script(){};

protected:
	Script(const std::string &name, GameObject *game_object) :Component(name, game_object){};
};

#endif // !__SCRIPT__
