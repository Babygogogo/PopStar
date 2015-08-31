#include "GetScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
struct GetScoreLabelScript::GetScoreLabelScriptImpl
{
	GetScoreLabelScriptImpl(GetScoreLabelScript *visitor);
	~GetScoreLabelScriptImpl();

	void onPlayerGetScore(const IEventData & e);
	void onLevelSummaryDisppeared(const IEventData & e);

	cocos2d::Label * getUnderlyingLabel() const;

	GetScoreLabelScript *m_Visitor{ nullptr };
};

GetScoreLabelScript::GetScoreLabelScriptImpl::GetScoreLabelScriptImpl(GetScoreLabelScript *visitor) : m_Visitor{ visitor }
{
}

GetScoreLabelScript::GetScoreLabelScriptImpl::~GetScoreLabelScriptImpl()
{
}

void GetScoreLabelScript::GetScoreLabelScriptImpl::onPlayerGetScore(const IEventData & e)
{
	auto exploded_stars_num = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getExplodedStarsNum());
	auto attained_score = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getScoreOfPreviousExplosion());

	auto underlyingLabel = getUnderlyingLabel();
	underlyingLabel->setString(std::string("Exploded: ") + exploded_stars_num + std::string(" Score: ") + attained_score);
	underlyingLabel->setVisible(true);
}

void GetScoreLabelScript::GetScoreLabelScriptImpl::onLevelSummaryDisppeared(const IEventData & e)
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

	eventDispatcher->vAddListener(EventType::PlayerGetScore, pimpl, [this](const IEventData & e){
		pimpl->onPlayerGetScore(e);
	});
	eventDispatcher->vAddListener(EventType::LevelSummaryDisappeared, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryDisppeared(e);
	});
}

const std::string GetScoreLabelScript::Type = "GetScoreLabelScript";
