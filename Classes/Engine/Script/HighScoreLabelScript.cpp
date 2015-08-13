#include "HighScoreLabelScript.h"
#include "cocos2d.h"
#include "../../Common/GameData.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of HighScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct HighScoreLabelScript::HighScoreLabelScriptImpl
{
	HighScoreLabelScriptImpl();
	~HighScoreLabelScriptImpl();

	std::string createHighScoreText() const;
};

HighScoreLabelScript::HighScoreLabelScriptImpl::HighScoreLabelScriptImpl()
{
}

HighScoreLabelScript::HighScoreLabelScriptImpl::~HighScoreLabelScriptImpl()
{
}

std::string HighScoreLabelScript::HighScoreLabelScriptImpl::createHighScoreText() const
{
	return std::string("High Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getHighScore());
}

//////////////////////////////////////////////////////////////////////////
//Implementation of HighScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
HighScoreLabelScript::HighScoreLabelScript() : pimpl{ std::make_unique<HighScoreLabelScriptImpl>() }
{
}

HighScoreLabelScript::~HighScoreLabelScript()
{
	if (auto & singletonContainer = SingletonContainer::getInstance())
		singletonContainer->get<IEventDispatcher>()->deleteListener(this);
}

void HighScoreLabelScript::vPostInit()
{
	auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
	labelUnderlying->setString(pimpl->createHighScoreText());

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::HighScoreValueUpdated, this, [this](BaseEventData*){
		auto labelUnderlying = this->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
		labelUnderlying->setString(pimpl->createHighScoreText());
	});
}

const std::string & HighScoreLabelScript::getType() const
{
	return Type;
}

const std::string HighScoreLabelScript::Type = "HighScoreLabelScript";
