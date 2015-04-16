#include <algorithm>

#include "GameObject.h"
#include "DisplayNode.h"
#include "../Script/TitleScene.h"

using std::unique_ptr;

template<typename Element, typename Container>
unique_ptr<Element> stealOwnership(Element *raw_ptr, Container &container)
{
	//find the ownership in container; if not found, return nullptr.
	auto iter_found = std::find_if(container.begin(), container.end(),
		[raw_ptr](const unique_ptr<Element>& p){return p.get() == raw_ptr;});
	if (iter_found == container.end())
		return nullptr;

	//steal the ownership
	iter_found->release();
	container.erase(iter_found);
	return unique_ptr<Element>(raw_ptr);
}

class GameObject::impl
{
public:
	impl(const std::string &name);
	~impl();

	GameObject *m_parent{ nullptr };
};

GameObject::impl::impl(const std::string &name)
{
}

GameObject::impl::~impl()
{
}

GameObject::GameObject(const std::string &name /*= ""*/) :Object(name), pimpl(new impl(name))
{

}

GameObject::~GameObject()
{
	CCLOG("GameObject %s destructing.", m_name.c_str());
}

void GameObject::addChild(std::unique_ptr<GameObject>&& child)
{
	if (!child || child->pimpl->m_parent)
		return;

	child->pimpl->m_parent = this;

	//deal with child's DisplayNode if present
	if (auto child_display_node = child->getComponent<DisplayNode>())
		this->addComponent<DisplayNode>()->addChild(child_display_node);
	
	m_children.emplace_back(std::move(child));
}

bool GameObject::isAncestorOf(const GameObject *child) const
{
	if (!child)
		return false;

	auto parent = child->pimpl->m_parent;
	while (parent){
		if (this == parent)
			return true;

		parent = parent->pimpl->m_parent;
	}

	return false;
}

GameObject * GameObject::getParent() const
{
	return pimpl->m_parent;
}

std::unique_ptr<GameObject> GameObject::getOwnershipFromParent()
{
	if (auto parent = getParent()){
		this->pimpl->m_parent = nullptr;
		if (auto display_node = getComponent<DisplayNode>())
			display_node->removeFromParent();

		return stealOwnership(this, parent->m_children);
	}

	return nullptr;
}
