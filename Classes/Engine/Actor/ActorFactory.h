#ifndef __ACTOR_FACTORY__
#define __ACTOR_FACTORY__

#include <memory>

#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

class Actor;

/*!
 * \class ActorFactory
 *
 * \brief A factory for creating Actors.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class ActorFactory
{
public:
	~ActorFactory();

	//Create the factory itself.
	static std::unique_ptr<ActorFactory> createFactory();

	//Create an Actor. The resourceFile is the file name of the corresponding .xml file.
	//modifyActor() will be called after the creation of the Actor.
	std::shared_ptr<Actor> createActor(const char *resourceFile, tinyxml2::XMLElement *overrides);

	//Modify an Actor with some xml data.
	//It will re-initialize its components and/or attach new ones to it.
	void modifyActor(const std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides);

private:
	ActorFactory();

	//Implementation stuff.
	struct ActorFactoryImpl;
	std::unique_ptr<ActorFactoryImpl> pimpl;
};

#endif // !__ACTOR_FACTORY__
