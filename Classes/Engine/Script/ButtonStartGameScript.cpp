#include "cocos2d.h"

#include "ButtonStartGameScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EvtDataGeneric.h"
#include "../GameLogic/GameLogic.h"
#include "../Graphic2D/SceneStack.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ButtonStartGameScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct ButtonStartGameScript::ButtonStartGameScriptImpl
{
	ButtonStartGameScriptImpl();
	~ButtonStartGameScriptImpl();

	void onClicked();
};

ButtonStartGameScript::ButtonStartGameScriptImpl::ButtonStartGameScriptImpl()
{
}

ButtonStartGameScript::ButtonStartGameScriptImpl::~ButtonStartGameScriptImpl()
{
}

void ButtonStartGameScript::ButtonStartGameScriptImpl::onClicked()
{
	auto & singletonContainer = SingletonContainer::getInstance();

	auto mainSceneActor = singletonContainer->get<GameLogic>()->createActor("Actors\\MainScene.xml");
	singletonContainer->get<SceneStack>()->replaceAndRun(*mainSceneActor);

	auto eventDispatcher = singletonContainer->get<IEventDispatcher>();
	eventDispatcher->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::NewGameStarted));
	eventDispatcher->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::LevelStarted));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ButtonStartGameScript.
//////////////////////////////////////////////////////////////////////////
ButtonStartGameScript::ButtonStartGameScript() : pimpl{ std::make_unique<ButtonStartGameScriptImpl>() }
{
}

ButtonStartGameScript::~ButtonStartGameScript()
{
}

void ButtonStartGameScript::vPostInit()
{
	auto menuItemImage = static_cast<cocos2d::MenuItemImage*>(m_Actor.lock()->getRenderComponent()->getSceneNode());
	menuItemImage->setCallback([this](cocos2d::Ref*){
		pimpl->onClicked();
	});
}

const std::string & ButtonStartGameScript::getType() const
{
	return Type;
}

const std::string ButtonStartGameScript::Type = "ButtonStartGameScript";
