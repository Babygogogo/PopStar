#include "LevelLabelScript.h"
#include "cocos2d.h"
#include "../../Common/GameData.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of LevelLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct LevelLabelScript::LevelLabelScriptImpl
{
	LevelLabelScriptImpl();
	~LevelLabelScriptImpl();

	std::string createLevelText() const;
};

LevelLabelScript::LevelLabelScriptImpl::LevelLabelScriptImpl()
{
}

LevelLabelScript::LevelLabelScriptImpl::~LevelLabelScriptImpl()
{
}

std::string LevelLabelScript::LevelLabelScriptImpl::createLevelText() const
{
	return std::string("Level: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel());
}

//////////////////////////////////////////////////////////////////////////
//Implementation of LevelLabelScript.
//////////////////////////////////////////////////////////////////////////
LevelLabelScript::LevelLabelScript() : pimpl{ std::make_unique<LevelLabelScriptImpl>() }
{
}

LevelLabelScript::~LevelLabelScript()
{
	if (auto & singletonContainer = SingletonContainer::getInstance())
		singletonContainer->get<IEventDispatcher>()->deleteListener(this);
}

void LevelLabelScript::vPostInit()
{
	auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
	labelUnderlying->setString(pimpl->createLevelText());

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::LevelValueUpdated, this, [this](BaseEventData*){
		auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
		labelUnderlying->setString(pimpl->createLevelText());
	});
}

const std::string & LevelLabelScript::getType() const
{
	return Type;
}

const std::string LevelLabelScript::Type = "LevelLabelScript";
