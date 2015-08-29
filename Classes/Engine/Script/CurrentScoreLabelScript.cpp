#include "cocos2d.h"

#include "CurrentScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataCurrentScoreValueUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CurrentScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CurrentScoreLabelScript::CurrentScoreLabelScriptImpl
{
	CurrentScoreLabelScriptImpl();
	~CurrentScoreLabelScriptImpl();

	cocos2d::Label *getUnderlyingLabel(const std::weak_ptr<Actor> & actor) const;
	std::string createCurrentScoreText(int newScore = 0) const;

	void onCurrentScoreValueUpdated(const IEventData & e, cocos2d::Label *underlyingLabel);
};

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::CurrentScoreLabelScriptImpl()
{
}

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::~CurrentScoreLabelScriptImpl()
{
}

cocos2d::Label * CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::getUnderlyingLabel(const std::weak_ptr<Actor> & actor) const
{
	return static_cast<cocos2d::Label*>(actor.lock()->getRenderComponent()->getSceneNode());
}

std::string CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::createCurrentScoreText(int currentScore /*= 0*/) const
{
	return std::string("Current Score: ") + std::to_string(currentScore);
}

void CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::onCurrentScoreValueUpdated(const IEventData & e, cocos2d::Label *underlyingLabel)
{
	auto newCurrentScore = (static_cast<const EvtDataCurrentScoreValueUpdated &>(e)).getNewCurrentScore();
	underlyingLabel->setString(createCurrentScoreText(newCurrentScore));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of CurrentScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
CurrentScoreLabelScript::CurrentScoreLabelScript() : pimpl{ std::make_shared<CurrentScoreLabelScriptImpl>() }
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::CurrentScoreValueUpdated, pimpl, [this](const IEventData & e){
		pimpl->onCurrentScoreValueUpdated(e, pimpl->getUnderlyingLabel(m_Actor));
	});
}

CurrentScoreLabelScript::~CurrentScoreLabelScript()
{
}

void CurrentScoreLabelScript::vPostInit()
{
	pimpl->getUnderlyingLabel(m_Actor)->setString(pimpl->createCurrentScoreText());
}

const std::string & CurrentScoreLabelScript::getType() const
{
	return Type;
}

const std::string CurrentScoreLabelScript::Type = "CurrentScoreLabelScript";
