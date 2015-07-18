#include <unordered_set>
#include <cassert>

#include "DisplayNode.h"
#include "Actor.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of DisplayNodeImpl.
//////////////////////////////////////////////////////////////////////////
struct DisplayNode::DisplayNodeImpl
{
public:
	DisplayNodeImpl();
	~DisplayNodeImpl();

	cocos2d::Node * m_Node{ nullptr };

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

	if (pimpl->m_Node){
		pimpl->m_Node->removeFromParent();
		pimpl->m_Node->release();
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

	if (child->pimpl->m_Node){
		if (this->pimpl->m_Node)
			this->pimpl->m_Node->addChild(child->pimpl->m_Node);
		else
			this->initAs<cocos2d::Node>()->addChild(child->pimpl->m_Node);
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
		if (parent->pimpl->m_Node)
			parent->pimpl->m_Node->addChild(this->pimpl->m_Node);
		else
			parent->initAs<cocos2d::Node>()->addChild(this->pimpl->m_Node);
	}
}

void DisplayNode::removeFromParent()
{
	if (!pimpl->m_parent || !pimpl->m_Node)
		return;

	pimpl->m_parent->pimpl->m_children.erase(this);
	pimpl->m_parent = nullptr;
	pimpl->m_Node->removeFromParent();
}

const std::string & DisplayNode::getType() const
{
	return Type;
}

bool DisplayNode::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void * DisplayNode::getNode() const
{
	return pimpl->m_Node;
}

void * DisplayNode::initAsHelper(std::function<void*()> && creatorFunction)
{
	//Ensure that the node hasn't been initialized.
	assert(!pimpl->m_Node);

	pimpl->m_Node = static_cast<cocos2d::Node*>(creatorFunction());
	pimpl->m_Node->retain();
	attachToParent();

	return pimpl->m_Node;
}

const std::string DisplayNode::Type = "DisplayNodeComponent";
