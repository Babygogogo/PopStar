#include "TitleSceneScript.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataGeneric.h"
#include "../GameLogic/GameLogic.h"
#include "../Utilities/SingletonContainer.h"
#include "../Graphic2D/SceneStack.h"
#include "../../Common/GameData.h"
#include "../Audio/Audio.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TitleSceneScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TitleSceneScript::TitleSceneScriptImpl
{
	TitleSceneScriptImpl();
	~TitleSceneScriptImpl();

	//HACK: It seems that the cocos2d-x engine has a mysterious bug here.
	//	If you create Menu with its factory method (e.g. auto menu = Menu::create(some_item..., NULL),
	//or auto menu = Menu::create(); menu->addChild(some_item);)
	//then all's right with the world.
	//	BUT if you create the menu and buttons with the GameObject and its components,
	//you will get a runtime error when you click the button, in line no. 128 of CCMenuItem.cpp.
	//	The cause is that the member CCMenuItem::_scriptType is changed somehow mysteriously,
	//without the script engine being initialized.
	//	To correct this, I made a change to the code of the engine, in line no. 124 of CCMenuItem.cpp:
	//before:	if (kScriptTypeNone != _scriptType)
	//after:	if (kScriptTypeNone != _scriptType && ScriptEngineManager::getInstance()->getScriptEngine())
	//	This change should be reasonable, but the better solution is to understand why the CCMenuItem::_scriptType is changed.
};

TitleSceneScript::TitleSceneScriptImpl::TitleSceneScriptImpl()
{
}

TitleSceneScript::TitleSceneScriptImpl::~TitleSceneScriptImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TitleSceneScript.
//////////////////////////////////////////////////////////////////////////
TitleSceneScript::TitleSceneScript() : pimpl{ std::make_unique<TitleSceneScriptImpl>() }
{
}

TitleSceneScript::~TitleSceneScript()
{
}

void TitleSceneScript::vPostInit()
{
	auto actor = m_Actor.lock();
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();

	//Add background and title menu.
	actor->addChild(gameLogic->createActor("Actors\\TitleSceneBackground.xml"));
	actor->addChild(gameLogic->createActor("Actors\\TitleMenu.xml"));

	Audio::getInstance()->playBGM();
}

const std::string & TitleSceneScript::getType() const
{
	return Type;
}

const std::string TitleSceneScript::Type = "TitleSceneScript";
