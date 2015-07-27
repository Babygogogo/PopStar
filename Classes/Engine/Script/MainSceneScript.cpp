#include "MainSceneScript.h"
#include "StatusLayer.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Audio/Audio.h"
#include "../GameLogic/GameLogic.h"
#include "../Utilities/SingletonContainer.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MainSceneScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MainSceneScript::MainSceneScriptImpl
{
	MainSceneScriptImpl();
	~MainSceneScriptImpl();
};

MainSceneScript::MainSceneScriptImpl::MainSceneScriptImpl()
{
}

MainSceneScript::MainSceneScriptImpl::~MainSceneScriptImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of MainSceneScript.
//////////////////////////////////////////////////////////////////////////
MainSceneScript::MainSceneScript() : pimpl{ std::make_unique<MainSceneScriptImpl>() }
{
}

MainSceneScript::~MainSceneScript()
{
}

const std::string & MainSceneScript::getType() const
{
	return Type;
}

void MainSceneScript::vPostInit()
{
	auto actor = m_Actor.lock();

	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	actor->addChild(gameLogic->createActor("Actors\\MainSceneBackground.xml"));
	actor->addChild(gameLogic->createActor("Actors\\MatrixLayer.xml"));

	auto statusLayerActor = std::make_shared<Actor>();
	statusLayerActor->addComponent<StatusLayer>();
	actor->addChild(std::move(statusLayerActor));

	Audio::getInstance()->playBGM();
}

const std::string MainSceneScript::Type = "MainSceneScript";
