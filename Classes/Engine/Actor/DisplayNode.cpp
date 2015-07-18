#include <unordered_set>

#include "DisplayNode.h"
#include "Actor.h"

//////////////////////////////////////////////////////////////////////////
//Definition of DisplayNodeImpl.
//////////////////////////////////////////////////////////////////////////
struct DisplayNode::DisplayNodeImpl
{
public:
	DisplayNodeImpl();
	~DisplayNodeImpl();

	DisplayNode* m_parent{ nullptr };
	std::unordered_set<DisplayNode*> m_children;
};

DisplayNode::DisplayNodeImpl::DisplayNodeImpl()
{

}

DisplayNode::DisplayNodeImpl::~DisplayNodeImpl()
{

}

//////////////////////////////////////////////////////////////////////////
//Implementation of DisplayNode.
//////////////////////////////////////////////////////////////////////////
DisplayNode::DisplayNode() : pimpl(new DisplayNodeImpl())
{

}

DisplayNode::DisplayNode(Actor *game_object) : ActorComponent("DisplayNode", game_object), pimpl(new DisplayNodeImpl())
{
	if (auto parent_object = game_object->getParent())
		parent_object->addComponent<DisplayNode>()->addChild(this);
}

DisplayNode::~DisplayNode()
{
	for (auto &child : pimpl->m_children)
		child->pimpl->m_parent = nullptr;

	if (m_node){
		m_node->removeFromParent();
		m_node->release();
	}

	if (pimpl->m_parent){
		pimpl->m_parent->pimpl->m_children.erase(this);
		pimpl->m_parent = nullptr;
	}
}

std::unique_ptr<DisplayNode> DisplayNode::create()
{
	return std::unique_ptr<DisplayNode>(new DisplayNode());
}

void DisplayNode::addChild(DisplayNode *child)
{
	if (!child || child->pimpl->m_parent)
		return;

	if (child->m_node){
		if (this->m_node)
			this->m_node->addChild(child->m_node);
		else
			this->initAs<cocos2d::Node>()->addChild(child->m_node);
	}

	child->pimpl->m_parent = this;
	pimpl->m_children.emplace(child);
}

DisplayNode * DisplayNode::getParent() const
{
	return pimpl->m_parent;
}

void DisplayNode::attachToParent()
{
	if (auto parent = getParent()){
		if (parent->m_node)
			parent->m_node->addChild(this->m_node);
		else
			parent->initAs<cocos2d::Node>()->addChild(this->m_node);
	}
}

void DisplayNode::removeFromParent()
{
	if (!pimpl->m_parent || !m_node)
		return;

	pimpl->m_parent->pimpl->m_children.erase(this);
	pimpl->m_parent = nullptr;
	m_node->removeFromParent();
}

const std::string & DisplayNode::getType() const
{
	return Type;
}

bool DisplayNode::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

const std::string DisplayNode::Type = "DisplayNodeComponent";
