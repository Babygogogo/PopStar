#include "cocos2d.h"

#include "TargetScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataTargetScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TargetScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TargetScoreLabelScript::TargetScoreLabelScriptImpl
{
	TargetScoreLabelScriptImpl(TargetScoreLabelScript *visitor);
	~TargetScoreLabelScriptImpl();

	void onTargetScoreValueUpdated(const IEventData & e);

	void setStringWithTargetScore(int targetScore = 0);

	TargetScoreLabelScript *m_Visitor{ nullptr };
};

TargetScoreLabelScript::TargetScoreLabelScriptImpl::TargetScoreLabelScriptImpl(TargetScoreLabelScript *visitor) : m_Visitor{ visitor }
{
}

TargetScoreLabelScript::TargetScoreLabelScriptImpl::~TargetScoreLabelScriptImpl()
{
}

void TargetScoreLabelScript::TargetScoreLabelScriptImpl::onTargetScoreValueUpdated(const IEventData & e)
{
	const auto & targetScoreEvent = static_cast<const EvtDataTargetScoreUpdated &>(e);
	setStringWithTargetScore(targetScoreEvent.getTargetScore());
}

void TargetScoreLabelScript::TargetScoreLabelScriptImpl::setStringWithTargetScore(int targetScore /*= 0*/)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode());
	underlyingLabel->setString(std::string("Target Score: ") + std::to_string(targetScore));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TargetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
TargetScoreLabelScript::TargetScoreLabelScript() : pimpl{ std::make_shared<TargetScoreLabelScriptImpl>(this) }
{
}

TargetScoreLabelScript::~TargetScoreLabelScript()
{
}

void TargetScoreLabelScript::vPostInit()
{
	pimpl->setStringWithTargetScore();

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::TargetScoreUpdated, pimpl, [this](const IEventData & e){
		pimpl->onTargetScoreValueUpdated(e);
	});
}

const std::string & TargetScoreLabelScript::getType() const
{
	return Type;
}

const std::string TargetScoreLabelScript::Type = "TargetScoreLabelScript";
