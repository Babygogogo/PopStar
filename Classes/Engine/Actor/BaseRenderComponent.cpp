#include "BaseRenderComponent.h"

BaseRenderComponent::BaseRenderComponent()
{
}

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}
