#ifndef __BW_COMPONENT__
#define __BW_COMPONENT__

#include "../Common/Object.h"

class GameObject;
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
class Component : public Object
{
public:
	virtual ~Component();

protected:
	Component(const std::string& name, GameObject *game_object);

	GameObject *m_game_object;
};

#endif // !__BW_COMPONENT__
