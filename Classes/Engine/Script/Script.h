#ifndef __SCRIPT__
#define __SCRIPT__

#include "../Actor/ActorComponent.h"

class Script :public ActorComponent
{
public:
	virtual ~Script(){};

protected:
	Script(std::string &&name, Actor *game_object) :ActorComponent(std::move(name), game_object){};
};

#endif // !__SCRIPT__
