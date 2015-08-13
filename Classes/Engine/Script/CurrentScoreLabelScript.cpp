#include "CurrentScoreLabelScript.h"
#include "cocos2d.h"
#include "../../Common/GameData.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CurrentScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CurrentScoreLabelScript::CurrentScoreLabelScriptImpl
{
	CurrentScoreLabelScriptImpl();
	~CurrentScoreLabelScriptImpl();

	std::string createCurrentScoreText() const;
};

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::CurrentScoreLabelScriptImpl()
{
}

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::~CurrentScoreLabelScriptImpl()
{
}

std::string CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::createCurrentScoreText() const
{
	return std::string("Current Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentScore());
}

//////////////////////////////////////////////////////////////////////////
//Implementation of CurrentScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
CurrentScoreLabelScript::CurrentScoreLabelScript() : pimpl{ std::make_unique<CurrentScoreLabelScriptImpl>() }
{
}

CurrentScoreLabelScript::~CurrentScoreLabelScript()
{
	if (auto & singletonContainer = SingletonContainer::getInstance())
		singletonContainer->get<IEventDispatcher>()->deleteListener(this);
}

void CurrentScoreLabelScript::vPostInit()
{
	auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
	labelUnderlying->setString(pimpl->createCurrentScoreText());

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::CurrentScoreValueUpdated, this, [this](BaseEventData*){
		auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
		labelUnderlying->setString(pimpl->createCurrentScoreText());
	});
}

const std::string & CurrentScoreLabelScript::getType() const
{
	return Type;
}

const std::string CurrentScoreLabelScript::Type = "CurrentScoreLabelScript";
