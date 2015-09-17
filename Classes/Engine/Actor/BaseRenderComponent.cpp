#include <cassert>

#include "cocos2d.h"

#include "BaseRenderComponent.h"
#include "../Utilities/RelativePosition.h"

BaseRenderComponent::~BaseRenderComponent()
{
	if (m_Node){
		m_Node->removeFromParent();
		m_Node->release();
		m_Node = nullptr;
	}
}

cocos2d::Node * BaseRenderComponent::getSceneNode() const
{
	return m_Node;
}

void BaseRenderComponent::setPosition(const RelativePosition & relativePosition)
{
	assert(m_Node && "BaseRenderComponent::setPosition() while the node is not initialized.");

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto screenPosX = relativePosition.m_NormalizedScreenOffsetX * visibleSize.width;
	auto screenPosY = relativePosition.m_NormalizedScreenOffsetY * visibleSize.height;

	auto nodeSizeX = m_Node->getContentSize().width;
	auto nodeSizeY = m_Node->getContentSize().height;
	auto nodePosX = relativePosition.m_NormalizedNodeOffsetX * nodeSizeX;
	auto nodePosY = relativePosition.m_NormalizedNodeOffsetY * nodeSizeY;

	m_Node->setPosition({ screenPosX + nodePosX + relativePosition.m_PixelOffsetX, screenPosY + nodePosY + relativePosition.m_PixelOffsetY });
}

void BaseRenderComponent::addChild(const BaseRenderComponent & child)
{
	assert(m_Node && child.m_Node && "BaseRenderComponent::addChild() parent or child is not initialized!");

	this->m_Node->addChild(child.m_Node);
}

void BaseRenderComponent::removeFromParent()
{
	assert(m_Node && "BaseRenderComponent::removeFromParent() the node is not initialized!");

	m_Node->removeFromParent();
}
