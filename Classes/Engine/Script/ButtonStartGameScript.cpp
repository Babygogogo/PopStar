#include "ButtonStartGameScript.h"
#include "../../Common/GameData.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EvtDataGeneric.h"
#include "../GameLogic/GameLogic.h"
#include "../Graphic2D/SceneStack.h"
#include "../Utilities/SingletonContainer.h"
#include "cocos2d.h"

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
	singletonContainer->get<GameData>()->reset();

	auto mainSceneActor = singletonContainer->get<GameLogic>()->createActor("Actors\\MainScene.xml");
	singletonContainer->get<SceneStack>()->replaceAndRun(std::move(mainSceneActor));

	singletonContainer->get<IEventDispatcher>()->dispatch(std::make_unique<EvtDataGeneric>(EventType::LevelStarted));
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
	auto menuItemImage = m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::MenuItemImage>();
	menuItemImage->setCallback([this](cocos2d::Ref*){
		pimpl->onClicked();
	});
}

const std::string & ButtonStartGameScript::getType() const
{
	return Type;
}

const std::string ButtonStartGameScript::Type = "ButtonStartGameScript";
