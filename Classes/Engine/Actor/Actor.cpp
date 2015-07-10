#include <algorithm>
#include <cassert>
#include <list>
#include <map>
#include <unordered_map>

#include "Actor.h"
#include "DisplayNode.h"

//////////////////////////////////////////////////////////////////////////
//Definition of Actor::ActorImpl.
//////////////////////////////////////////////////////////////////////////
struct Actor::ActorImpl
{
public:
	ActorImpl();
	ActorImpl(ActorID && id, std::string && type, std::string && resourceFile);
	~ActorImpl();

	template<typename Element, typename Container>
	std::unique_ptr<Element> stealOwnership(Element *raw_ptr, Container &container)
	{
		//find the ownership in container; if not found, return nullptr.
		auto iter_found = std::find_if(container.begin(), container.end(),
			[raw_ptr](const std::unique_ptr<Element>& p){return p.get() == raw_ptr; });
		if (iter_found == container.end())
			return nullptr;

		//steal the ownership
		iter_found->release();
		container.erase(iter_found);
		return std::unique_ptr<Element>(raw_ptr);
	}

	std::list<std::unique_ptr<Actor>> m_children;
	std::map<Actor*, bool> m_children_deletion_flag;
	Actor *m_parent{ nullptr };
	bool m_is_updating{ false };
	bool m_is_need_update{ true };

	ActorID m_ID{ 0 };
	std::string m_Type;
	std::string m_ResourceFile;
	std::unordered_map<std::string, std::unique_ptr<ActorComponent>> m_Components;
};

Actor::ActorImpl::ActorImpl()
{

}

Actor::ActorImpl::ActorImpl(ActorID && id, std::string && type, std::string && resourceFile) : m_ID(std::move(id)), m_Type(std::move(type)), m_ResourceFile(std::move(resourceFile))
{

}

Actor::ActorImpl::~ActorImpl()
{

}

//////////////////////////////////////////////////////////////////////////
//Implementation of Actor.
//////////////////////////////////////////////////////////////////////////
Actor::Actor() : pimpl(new ActorImpl())
{

}

Actor::Actor(std::string &&name) : pimpl(new ActorImpl())
{

}

Actor::Actor(ActorID && id, std::string && type, std::string && resourceFile) : pimpl(new ActorImpl(std::move(id), std::move(type), std::move(resourceFile)))
{

}

Actor::~Actor()
{
	cocos2d::log("GameObject %s destructing.", pimpl->m_Type.c_str());
}

Actor * Actor::addChild(std::unique_ptr<Actor>&& child)
{
	if (!child || child->pimpl->m_parent)
		return nullptr;

	child->pimpl->m_parent = this;

	//deal with child's DisplayNode if present
	if (auto child_display_node = child->getComponent<DisplayNode>())
		this->addComponent<DisplayNode>()->addChild(child_display_node);
	
	pimpl->m_children_deletion_flag.emplace(child.get(), false);
	pimpl->m_children.emplace_back(std::move(child));
	
	return pimpl->m_children.back().get();
}

bool Actor::isAncestorOf(const Actor *child) const
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

Actor * Actor::getParent() const
{
	return pimpl->m_parent;
}

std::unique_ptr<Actor> Actor::removeFromParent()
{
	if (auto parent = getParent()){
		this->pimpl->m_parent = nullptr;
		if (auto display_node = getComponent<DisplayNode>())
			display_node->removeFromParent();

		if (pimpl->m_is_updating)
			parent->pimpl->m_children_deletion_flag[this] = true;
		else
			parent->pimpl->m_children_deletion_flag.erase(this);

		return pimpl->stealOwnership(this, parent->pimpl->m_children);
	}

	return nullptr;
}

void Actor::update(const time_t &time_ms)
{
	if (!pimpl->m_is_need_update)
		return;

	pimpl->m_is_updating = true;

	for (auto &updateable : m_updateable_components)
		updateable->update(time_ms);

	for (auto child_it = pimpl->m_children_deletion_flag.begin(); child_it != pimpl->m_children_deletion_flag.end();){
		if (child_it->second){
			child_it = pimpl->m_children_deletion_flag.erase(child_it);
			continue;
		}

		(child_it++)->first->update(time_ms);
	}

	pimpl->m_is_updating = false;
}

void Actor::setNeedUpdate(bool is_need)
{
	pimpl->m_is_need_update = is_need;
}

std::shared_ptr<Actor> Actor::create()
{
	return std::unique_ptr<Actor>(new Actor());
}

std::shared_ptr<Actor> Actor::create(ActorID id, std::string && type, std::string && resourceFile)
{
	return std::unique_ptr<Actor>(new Actor(std::move(id), std::move(type), std::move(resourceFile)));
}

bool Actor::init(ActorID id, tinyxml2::XMLElement *xmlElement)
{
	if (id == 0 || !xmlElement)
		return false;

	auto actorType = xmlElement->Attribute("type");
	if (!actorType){
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	auto resourceFile = xmlElement->Attribute("resource");
	if (!resourceFile){
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	pimpl->m_ID = std::move(id);
	pimpl->m_Type = actorType;
	pimpl->m_ResourceFile = resourceFile;

	return true;
}

void Actor::postInit()
{
	for (auto & componentIter : pimpl->m_Components)
		componentIter.second->vPostInit();
}

void Actor::addComponent(std::unique_ptr<ActorComponent> && component)
{
	assert(component);
	auto emplaceResult = pimpl->m_Components.emplace(std::make_pair(component->getType(), std::move(component)));
	assert(emplaceResult.second);
}

const std::unique_ptr<ActorComponent> & Actor::getComponent(const std::string & type) const
{
	auto findIter = pimpl->m_Components.find(type);
	if (findIter == pimpl->m_Components.end()){
		static std::unique_ptr < ActorComponent > emptyComponent;
		return emptyComponent;
	}

	return findIter->second;
}
