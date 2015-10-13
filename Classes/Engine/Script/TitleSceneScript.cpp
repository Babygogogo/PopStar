#include "TitleSceneScript.h"
#include "../Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TitleSceneScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TitleSceneScript::TitleSceneScriptImpl
{
	TitleSceneScriptImpl();
	~TitleSceneScriptImpl();
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
	Audio::getInstance()->playBGM();
}

const std::string & TitleSceneScript::getType() const
{
	return Type;
}

const std::string TitleSceneScript::Type = "TitleSceneScript";
