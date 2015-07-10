#ifndef __ACTOR_FACTORY__
#define __ACTOR_FACTORY__

#include <memory>

#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

class Actor;

class ActorFactory
{
public:
	~ActorFactory();

	static std::unique_ptr<ActorFactory> createFactory();

	std::shared_ptr<Actor> createActor(const char *resourceFile, tinyxml2::XMLElement *overrides);

	void modifyActor(std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides);

private:
	ActorFactory();

	//Implementation stuff.
	struct ActorFactoryImpl;
	std::unique_ptr<ActorFactoryImpl> pimpl;
};

#endif // !__ACTOR_FACTORY__
