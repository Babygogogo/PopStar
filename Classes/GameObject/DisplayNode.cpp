#include "DisplayNode.h"
#include "../GameObject/GameObject.h"
#include <unordered_set>

using std::unordered_set;

class DisplayNode::impl
{
public:
	impl(GameObject *game_object);
	~impl();

	DisplayNode* m_parent{ nullptr };
	unordered_set<DisplayNode*> m_children;
};

DisplayNode::impl::impl(GameObject *game_object)
{
}

DisplayNode::impl::~impl()
{
}

DisplayNode::DisplayNode(GameObject *game_object) : Component("DisplayNode", game_object), pimpl(new impl(game_object))
{
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

void DisplayNode::addChild(DisplayNode *child)
{
	if (!child || child->pimpl->m_parent)
		return;

	if (child->m_node){
		if (this->m_node)
			this->m_node->addChild(child->m_node);
		else
			this->initAs<DefaultNode>()->addChild(child->m_node);
	}

	child->pimpl->m_parent = this;
	pimpl->m_children.emplace(child);
}

DisplayNode * DisplayNode::getParent() const
{
	return pimpl->m_parent;
}

void DisplayNode::removeFromParent()
{
	if (!pimpl->m_parent || !m_node)
		return;

	pimpl->m_parent->pimpl->m_children.erase(this);
	pimpl->m_parent = nullptr;
	m_node->removeFromParent();
}
