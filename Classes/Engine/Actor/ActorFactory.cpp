#include "ActorFactory.h"
#include "Actor.h"
#include "ActorComponent.h"
#include "../Utilities/GenericFactory.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActorFactory::ActorFactoryImpl.
//////////////////////////////////////////////////////////////////////////
struct ActorFactory::ActorFactoryImpl
{
	ActorFactoryImpl();
	~ActorFactoryImpl();

	std::unique_ptr<ActorComponent> createComponent(tinyxml2::XMLElement * componentElement);
	void postInitActor(std::shared_ptr<Actor> & actor);

	Actor::ActorID getNextID() const;
	void updateID();

	Actor::ActorID m_currentID{ 0 };
	std::unique_ptr<GenericFactory<ActorComponent>> m_ComponentFactory;
};

ActorFactory::ActorFactoryImpl::ActorFactoryImpl()
{

}

ActorFactory::ActorFactoryImpl::~ActorFactoryImpl()
{

}

std::unique_ptr<ActorComponent> ActorFactory::ActorFactoryImpl::createComponent(tinyxml2::XMLElement * componentElement)
{
	auto componentType = componentElement->Value();
	auto component = m_ComponentFactory->createObject(componentType);

	// initialize the component if we found one
	if (component){
		if (!component->vInit(componentElement)){
			cocos2d::log("ActorFactoryImpl::createComponent failed to create a(n) %s", componentType);
			return nullptr;
		}
	} else {
		cocos2d::log("ActorFactoryImpl::createComponent couldn't find an ActorComponent named %s", componentType);
		return nullptr;
	}

	return component;
}

void ActorFactory::ActorFactoryImpl::postInitActor(std::shared_ptr<Actor> & actor)
{
	//If you want to post-init the components in some order, do it here.
	actor->postInit();
}

Actor::ActorID ActorFactory::ActorFactoryImpl::getNextID() const
{
	return m_currentID + 1;
}

void ActorFactory::ActorFactoryImpl::updateID()
{
	++m_currentID;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ActorFactory.
//////////////////////////////////////////////////////////////////////////
ActorFactory::ActorFactory() : pimpl(new ActorFactoryImpl())
{

}

ActorFactory::~ActorFactory()
{

}

std::unique_ptr<ActorFactory> ActorFactory::createFactory()
{
	return std::unique_ptr<ActorFactory>(new ActorFactory());
}

std::shared_ptr<Actor> ActorFactory::createActor(const char *resourceFile, tinyxml2::XMLElement *overrides)
{
	//Load the resource file. If failed, log and return nullptr.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(resourceFile);
	const auto rootElement = xmlDoc.RootElement();
	if (!rootElement){
		cocos2d::log("ActorFactory::createActor failed to load resource file %s", resourceFile);
		return nullptr;
	}

	//Create and init the actor. If failed, log and return nullptr.
	auto actor = Actor::create();
	if (!actor || !actor->init(pimpl->getNextID(), rootElement)){
		cocos2d::log("ActorFactory::createActor failed to create or init an actor.");
		return nullptr;
	}

	//Loop through each child element and load the component
	for (auto componentElement = rootElement->FirstChildElement(); componentElement; componentElement = componentElement->NextSiblingElement()){
		auto component = pimpl->createComponent(componentElement);
	
		//If failed to create a component, return nullptr because the actor will be partially complete and may cause more troubles than goods.
		if (!component)
			return nullptr;

		//If the component is created successfully, add it to the actor.
		component->setOwner(actor);
		actor->addComponent(std::move(component));
	}

	//Modify the actor using data in the overrides and then post-init it.
	modifyActor(actor, overrides);
	pimpl->postInitActor(actor);

	//Finally, the creation succeed. Update the m_NextID and return the created actor.
	pimpl->updateID();
	return actor;
}

void ActorFactory::modifyActor(std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides)
{
	if (!overrides)
		return;

	// Loop through each child element and load the component
	for (auto componentElement = overrides->FirstChildElement(); componentElement; componentElement = componentElement->NextSiblingElement()){
		auto & component = actor->getComponent(componentElement->Value());
		
		if (component){
			component->vInit(componentElement);
			component->vOnChanged();
		} else {
			auto newComponent = pimpl->createComponent(componentElement);
			if (newComponent){
				component->setOwner(actor);
				actor->addComponent(std::move(newComponent));
			}
		}
	}
}
