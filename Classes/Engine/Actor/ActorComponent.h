#ifndef __ACTOR_COMPONENT__
#define __ACTOR_COMPONENT__

#include <string>
#include <memory>

#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

/*!
 * \brief The base class of every components and scripts.
 *
 * \details
 *	Refactor in progress...
 *
 * \author Babygogogo
 * \date 2015.3
 */
class ActorComponent
{
	friend class Actor;
	friend class ActorFactory;

public:
	virtual ~ActorComponent();

	virtual const std::string & getType() const = 0;

protected:
	ActorComponent();
	ActorComponent(std::string &&name, Actor *game_object);

	virtual bool vInit(tinyxml2::XMLElement *xmlElement) = 0;
	virtual void vPostInit();
	virtual void vOnChanged();

	Actor *m_game_object;
	std::weak_ptr<Actor> m_Actor;

public:
	//This function should really be private and only exposed to ActorFactory.
	//It's now public so that I can refactor the components more conveniently.
	//When the refactor is done, this function should again be private.
	void setOwner(std::weak_ptr<Actor> && owner);
};

#endif // !__ACTOR_COMPONENT__
