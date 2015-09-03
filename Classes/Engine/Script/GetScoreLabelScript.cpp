#include "cocos2d.h"

#include "GetScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataPlayerGotScore.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
struct GetScoreLabelScript::GetScoreLabelScriptImpl
{
	GetScoreLabelScriptImpl(GetScoreLabelScript *visitor);
	~GetScoreLabelScriptImpl();

	void onPlayerGotScore(const IEventData & e);
	void onLevelSummaryFinished(const IEventData & e);

	cocos2d::Label * getUnderlyingLabel() const;

	GetScoreLabelScript *m_Visitor{ nullptr };
};

GetScoreLabelScript::GetScoreLabelScriptImpl::GetScoreLabelScriptImpl(GetScoreLabelScript *visitor) : m_Visitor{ visitor }
{
}

GetScoreLabelScript::GetScoreLabelScriptImpl::~GetScoreLabelScriptImpl()
{
}

void GetScoreLabelScript::GetScoreLabelScriptImpl::onPlayerGotScore(const IEventData & e)
{
	auto & playerGotScoreEvent = static_cast<const EvtDataPlayerGotScore &>(e);
	auto explodedStarsCountString = std::string("Exploded: ") + std::to_string(playerGotScoreEvent.getExplodedStarsCount());
	auto gotScore = std::string(" Score: ") + std::to_string(playerGotScoreEvent.getScore());

	auto underlyingLabel = getUnderlyingLabel();
	underlyingLabel->setString(explodedStarsCountString + gotScore);
	underlyingLabel->setVisible(true);
}

void GetScoreLabelScript::GetScoreLabelScriptImpl::onLevelSummaryFinished(const IEventData & e)
{
	getUnderlyingLabel()->setVisible(false);
}

cocos2d::Label * GetScoreLabelScript::GetScoreLabelScriptImpl::getUnderlyingLabel() const
{
	return static_cast<cocos2d::Label*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode());
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
GetScoreLabelScript::GetScoreLabelScript() : pimpl{ std::make_shared<GetScoreLabelScriptImpl>(this) }
{
}

GetScoreLabelScript::~GetScoreLabelScript()
{
}

const std::string & GetScoreLabelScript::getType() const
{
	return Type;
}

void GetScoreLabelScript::vPostInit()
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();

	eventDispatcher->vAddListener(EventType::PlayerGotScore, pimpl, [this](const IEventData & e){
		pimpl->onPlayerGotScore(e);
	});
	eventDispatcher->vAddListener(EventType::LevelSummaryFinished, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryFinished(e);
	});
}

const std::string GetScoreLabelScript::Type = "GetScoreLabelScript";
