#include <algorithm>
#include <cassert>
#include <list>
#include <map>
#include <unordered_map>

#include "Actor.h"
#include "GeneralRenderComponent.h"
#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of Actor::ActorImpl.
//////////////////////////////////////////////////////////////////////////
struct Actor::ActorImpl
{
public:
	ActorImpl();
	~ActorImpl();

	template<typename Element, typename Container>
	//	std::unique_ptr<Element> stealOwnership(Element *raw_ptr, Container &container)
	std::shared_ptr<Element> stealOwnership(Element *raw_ptr, Container &container)
	{
		//find the ownership in container; if not found, return nullptr.
		auto iter_found = std::find_if(container.begin(), container.end(),
			//			[raw_ptr](const std::unique_ptr<Element>& p){return p.get() == raw_ptr; });
			[raw_ptr](const std::shared_ptr<Element>& p){return p.get() == raw_ptr; });
		if (iter_found == container.end())
			return nullptr;

		//steal the ownership
		//iter_found->release();
		//container.erase(iter_found);
		//return std::unique_ptr<Element>(raw_ptr);
		auto elementFound = *iter_found;
		container.erase(iter_found);
		return elementFound;
	}

	//	std::list<std::unique_ptr<Actor>> m_children;
	std::list<std::shared_ptr<Actor>> m_children;
	std::map<Actor*, bool> m_children_deletion_flag;
	Actor *m_parent{ nullptr };
	bool m_is_updating{ false };

	ActorID m_ID{ 0 };
	std::string m_Type;
	std::string m_ResourceFile;
	std::unordered_map<std::string, std::shared_ptr<ActorComponent>> m_Components;
};

Actor::ActorImpl::ActorImpl()
{
}

Actor::ActorImpl::~ActorImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of Actor.
//////////////////////////////////////////////////////////////////////////
Actor::Actor() : pimpl{ std::make_unique<ActorImpl>() }
{
}

Actor::~Actor()
{
	cocos2d::log("GameObject %s destructing.", pimpl->m_Type.c_str());
}

std::weak_ptr<Actor> Actor::addChild(std::shared_ptr<Actor> && child)
{
	if (!child || child->pimpl->m_parent)
		return child;

	child->pimpl->m_parent = this;

	//deal with child's DisplayNode if present
	if (auto child_display_node = child->getComponent<GeneralRenderComponent>())
		this->getComponent<GeneralRenderComponent>()->addChild(child_display_node.get());

	pimpl->m_children_deletion_flag.emplace(child.get(), false);
	pimpl->m_children.emplace_back(std::move(child));

	//	return pimpl->m_children.back().get();
	return pimpl->m_children.back();
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

std::shared_ptr<Actor> Actor::removeFromParent()
{
	if (auto parent = getParent()){
		this->pimpl->m_parent = nullptr;
		if (auto display_node = getComponent<GeneralRenderComponent>())
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
	pimpl->m_is_updating = true;

	for (auto child_it = pimpl->m_children_deletion_flag.begin(); child_it != pimpl->m_children_deletion_flag.end();){
		if (child_it->second){
			child_it = pimpl->m_children_deletion_flag.erase(child_it);
			continue;
		}

		(child_it++)->first->update(time_ms);
	}

	pimpl->m_is_updating = false;
}

void Actor::update(const std::chrono::milliseconds & delteTimeMs)
{
	//#TODO: call update() on all components here.
}

bool Actor::init(ActorID id, tinyxml2::XMLElement *xmlElement)
{
	if (id == 0 || !xmlElement)
		return false;

	auto actorType = xmlElement->Attribute("Type");
	if (!actorType){
		cocos2d::log("Actor::init failed because there's no type attribute in resource.");
		return false;
	}

	auto resourceFile = xmlElement->Attribute("Resource");
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

void Actor::addComponent(std::shared_ptr<ActorComponent> && component)
{
	//Ensure that the component is alive.
	assert(component);

	//#TODO: Check if the component is an render component, and make sure that the actor can have no more than one render component.

	auto emplaceResult = pimpl->m_Components.emplace(std::make_pair(component->getType(), std::move(component)));
	assert(emplaceResult.second);
}

std::shared_ptr<ActorComponent> Actor::getComponent(const std::string & type) const
{
	auto findIter = pimpl->m_Components.find(type);
	if (findIter == pimpl->m_Components.end())
		return nullptr;

	return findIter->second;
}

const ActorID & Actor::getID() const
{
	return pimpl->m_ID;
}
