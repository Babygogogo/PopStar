#include "TargetScoreLabelScript.h"
#include "cocos2d.h"
#include "../../Common/GameData.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TargetScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TargetScoreLabelScript::TargetScoreLabelScriptImpl
{
	TargetScoreLabelScriptImpl();
	~TargetScoreLabelScriptImpl();

	std::string createTargetScoreText() const;
};

TargetScoreLabelScript::TargetScoreLabelScriptImpl::TargetScoreLabelScriptImpl()
{
}

TargetScoreLabelScript::TargetScoreLabelScriptImpl::~TargetScoreLabelScriptImpl()
{
}

std::string TargetScoreLabelScript::TargetScoreLabelScriptImpl::createTargetScoreText() const
{
	return std::string("Target Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getTargetScore());
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TargetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
TargetScoreLabelScript::TargetScoreLabelScript() : pimpl{ std::make_unique<TargetScoreLabelScriptImpl>() }
{
}

TargetScoreLabelScript::~TargetScoreLabelScript()
{
	if (auto & singletonContainer = SingletonContainer::getInstance())
		singletonContainer->get<IEventDispatcher>()->deleteListener(this);
}

void TargetScoreLabelScript::vPostInit()
{
	auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
	labelUnderlying->setString(pimpl->createTargetScoreText());

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::TargetScoreValueUpdated, this, [this](BaseEventData*){
		auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
		labelUnderlying->setString(pimpl->createTargetScoreText());
	});
}

const std::string & TargetScoreLabelScript::getType() const
{
	return Type;
}

const std::string TargetScoreLabelScript::Type = "TargetScoreLabelScript";
