#ifndef __SCRIPT__
#define __SCRIPT__

#include "../GameObject/Component.h"

class Script :public Component
{
public:
	virtual ~Script(){};

protected:
	Script(const std::string &name, GameObject *game_object) :Component(name, game_object){};
};

#endif // !__SCRIPT__
