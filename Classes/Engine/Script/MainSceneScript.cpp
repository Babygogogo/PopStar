#include "MainSceneScript.h"
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
	Audio::getInstance()->playBGM();
}

const std::string MainSceneScript::Type = "MainSceneScript";
