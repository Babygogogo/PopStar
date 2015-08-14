#ifndef __BASE_RENDER_COMPONENT__
#define __BASE_RENDER_COMPONENT__

#include "ActorComponent.h"

//Forward declaration.
namespace cocos2d
{
	class Node;
}

/*!
 * \class BaseRenderComponent
 *
 * \brief The base class of all other render components.
 *
 * \details
 *	For simplicity, every actor can have no more than one concrete render component.
 *
 * \author Babygogogo
 * \date 2015.7
 */
class BaseRenderComponent : public ActorComponent
{
public:
	cocos2d::Node * getSceneNode() const;

	//Disable copy/move constructor and operator=.
	BaseRenderComponent(const BaseRenderComponent &) = delete;
	BaseRenderComponent(BaseRenderComponent &&) = delete;
	BaseRenderComponent & operator=(const BaseRenderComponent &) = delete;
	BaseRenderComponent & operator=(BaseRenderComponent &&) = delete;

protected:
	BaseRenderComponent();

	cocos2d::Node *m_Node{ nullptr };
};

#endif // !__BASE_RENDER_COMPONENT__
