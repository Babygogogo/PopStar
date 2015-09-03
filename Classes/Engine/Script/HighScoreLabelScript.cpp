#include "cocos2d.h"

#include "HighScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataHighScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of HighScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct HighScoreLabelScript::HighScoreLabelScriptImpl
{
	HighScoreLabelScriptImpl(HighScoreLabelScript *visitor);
	~HighScoreLabelScriptImpl();

	void setStringWithScore(int highScore = 0);

	void onHighScoreUpdated(const IEventData & e);

	HighScoreLabelScript *m_Visitor{ nullptr };
};

HighScoreLabelScript::HighScoreLabelScriptImpl::HighScoreLabelScriptImpl(HighScoreLabelScript *visitor) : m_Visitor{ visitor }
{
}

HighScoreLabelScript::HighScoreLabelScriptImpl::~HighScoreLabelScriptImpl()
{
}

void HighScoreLabelScript::HighScoreLabelScriptImpl::onHighScoreUpdated(const IEventData & e)
{
	auto & highScoreEvent = static_cast<const EvtDataHighScoreUpdated &>(e);
	setStringWithScore(highScoreEvent.getHighScore());
}

void HighScoreLabelScript::HighScoreLabelScriptImpl::setStringWithScore(int highScore /*= 0*/)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode());
	underlyingLabel->setString(std::string("High Score: ") + std::to_string(highScore));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of HighScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
HighScoreLabelScript::HighScoreLabelScript() : pimpl{ std::make_shared<HighScoreLabelScriptImpl>(this) }
{
}

HighScoreLabelScript::~HighScoreLabelScript()
{
}

void HighScoreLabelScript::vPostInit()
{
	pimpl->setStringWithScore();

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::HighScoreUpdated, pimpl, [this](const IEventData & e){
		pimpl->onHighScoreUpdated(e);
	});
}

const std::string & HighScoreLabelScript::getType() const
{
	return Type;
}

const std::string HighScoreLabelScript::Type = "HighScoreLabelScript";
