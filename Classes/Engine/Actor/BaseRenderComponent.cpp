#include <cassert>

#include "cocos2d.h"

#include "BaseRenderComponent.h"

BaseRenderComponent::~BaseRenderComponent()
{
	if (m_Node){
		m_Node->removeFromParent();
		m_Node->release();
		m_Node = nullptr;
	}
}

void BaseRenderComponent::addChild(const BaseRenderComponent & child)
{
	assert(this->m_Node && child.m_Node && "BaseRenderComponent::addChild() parent or child is not initialized!");

	this->m_Node->addChild(child.m_Node);
}

void BaseRenderComponent::removeFromParent()
{
	assert(m_Node && "BaseRenderComponent::removeFromParent() the node is not initialized!");

	m_Node->removeFromParent();
}

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}
