#include "ActorFactory.h"
#include "Actor.h"
#include "ActorID.h"
#include "ActorComponent.h"
#include "DisplayNode.h"
#include "../Script/ComboEffectScript.h"
#include "../Utilities/GenericFactory.h"
#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ActorFactory::ActorFactoryImpl.
//////////////////////////////////////////////////////////////////////////
struct ActorFactory::ActorFactoryImpl
{
	ActorFactoryImpl();
	~ActorFactoryImpl();

	//This function is called in the constructor of impl.
	//You must modify this function whenever the types of components are changed.
	void registerComponents();

	std::unique_ptr<ActorComponent> createComponent(tinyxml2::XMLElement * componentElement);
	void postInitActor(std::shared_ptr<Actor> & actor);

	ActorID getNextID() const;
	void updateID();

	ActorID m_currentID{ 0 };
	GenericFactory<ActorComponent> m_ComponentFactory;
};

ActorFactory::ActorFactoryImpl::ActorFactoryImpl()
{
	registerComponents();
}

ActorFactory::ActorFactoryImpl::~ActorFactoryImpl()
{
}

void ActorFactory::ActorFactoryImpl::registerComponents()
{
	//#TODO: Register all of the concrete components here.
	m_ComponentFactory.registerType<DisplayNode>();
	m_ComponentFactory.registerType<ComboEffectScript>();
}

std::unique_ptr<ActorComponent> ActorFactory::ActorFactoryImpl::createComponent(tinyxml2::XMLElement * componentElement)
{
	auto componentType = componentElement->Value();
	auto component = m_ComponentFactory.createUnique(componentType);

	//If can't create the component, log and return nullptr.
	if (!component){
		cocos2d::log("ActorFactoryImpl::createComponent couldn't find an ActorComponent named %s", componentType);
		return nullptr;
	}

	//Try to initialize the component. If it fails, log and return nullptr.
	if (!component->vInit(componentElement)){
		cocos2d::log("ActorFactoryImpl::createComponent failed to initialize a(n) %s", componentType);
		return nullptr;
	}

	//The component is created and initialized successfully.
	return component;
}

void ActorFactory::ActorFactoryImpl::postInitActor(std::shared_ptr<Actor> & actor)
{
	//If you want to post-init the components in some order, do it here.
	actor->postInit();
}

ActorID ActorFactory::ActorFactoryImpl::getNextID() const
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
ActorFactory::ActorFactory() : pimpl{ std::make_unique<ActorFactoryImpl>() }
{
}

ActorFactory::~ActorFactory()
{
}

std::shared_ptr<Actor> ActorFactory::createActor(const char *resourceFile, tinyxml2::XMLElement *overrides /*= nullptr*/)
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
	auto actor = std::make_shared<Actor>();
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

		//The component is created successfully so add it to the actor.
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

void ActorFactory::modifyActor(const std::shared_ptr<Actor> & actor, tinyxml2::XMLElement *overrides)
{
	if (!actor || !overrides)
		return;

	// Loop through each child element and load the component
	for (auto componentElement = overrides->FirstChildElement(); componentElement; componentElement = componentElement->NextSiblingElement()){
		auto & component = actor->getComponent(componentElement->Value());

		//If there is a component of the same type already, re-initialize it.
		if (component){
			component->vInit(componentElement);
			component->vOnChanged();
		}
		else {
			//Else, create a new component and attach to actor.
			auto newComponent = pimpl->createComponent(componentElement);
			if (newComponent){
				component->setOwner(actor);
				actor->addComponent(std::move(newComponent));
			}
		}
	}
}
