#include "cocos2d.h"

#include "CurrentScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataCurrentScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CurrentScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CurrentScoreLabelScript::CurrentScoreLabelScriptImpl
{
	CurrentScoreLabelScriptImpl(CurrentScoreLabelScript *visitor);
	~CurrentScoreLabelScriptImpl();

	void setStringWithScore(int currentScore = 0);
	void onCurrentScoreValueUpdated(const IEventData & e);

	CurrentScoreLabelScript *m_Visitor{ nullptr };
};

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::CurrentScoreLabelScriptImpl(CurrentScoreLabelScript *visitor) : m_Visitor{ visitor }
{
}

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::~CurrentScoreLabelScriptImpl()
{
}

void CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::setStringWithScore(int currentScore /*= 0*/)
{
	auto scoreText = std::string("Current Score: ") + std::to_string(currentScore);
	static_cast<cocos2d::Label*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode())->setString(scoreText);
}

void CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::onCurrentScoreValueUpdated(const IEventData & e)
{
	auto newCurrentScore = (static_cast<const EvtDataCurrentScoreUpdated &>(e)).getNewCurrentScore();
	setStringWithScore(newCurrentScore);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of CurrentScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
CurrentScoreLabelScript::CurrentScoreLabelScript() : pimpl{ std::make_shared<CurrentScoreLabelScriptImpl>(this) }
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::CurrentScoreUpdated, pimpl, [this](const IEventData & e){
		pimpl->onCurrentScoreValueUpdated(e);
	});
}

CurrentScoreLabelScript::~CurrentScoreLabelScript()
{
}

void CurrentScoreLabelScript::vPostInit()
{
	pimpl->setStringWithScore();
}

const std::string & CurrentScoreLabelScript::getType() const
{
	return Type;
}

const std::string CurrentScoreLabelScript::Type = "CurrentScoreLabelScript";
