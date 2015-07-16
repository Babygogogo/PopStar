#ifndef __ACTOR_COMPONENT__
#define __ACTOR_COMPONENT__

#include <string>
#include <memory>

#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

/*!
 * \brief The base class of every components and scripts.
 *
 * \details
 *	Components can be created only by GameObject.addComponent<ConcreteComponent>().
 *	Concrete components must have a static canUpdate boolean that indicates whether it has a update method or not.
 *	If canUpdate is true, a update method must also be provided by the concrete component.
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

private:
	void setOwner(std::weak_ptr<Actor> && actor);
};

#endif // !__ACTOR_COMPONENT__
