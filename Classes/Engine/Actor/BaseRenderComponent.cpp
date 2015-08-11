#include "BaseRenderComponent.h"

BaseRenderComponent::BaseRenderComponent()
{

}

BaseRenderComponent::BaseRenderComponent(std::string && name, Actor *game_object) : ActorComponent(std::move(name), game_object)
{
}

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}
