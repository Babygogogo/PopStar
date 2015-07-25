#include "GetScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
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
	GetScoreLabelScriptImpl();
	~GetScoreLabelScriptImpl();
};

GetScoreLabelScript::GetScoreLabelScriptImpl::GetScoreLabelScriptImpl()
{
}

GetScoreLabelScript::GetScoreLabelScriptImpl::~GetScoreLabelScriptImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
GetScoreLabelScript::GetScoreLabelScript() : pimpl{ std::make_unique<GetScoreLabelScriptImpl>() }
{
}

GetScoreLabelScript::~GetScoreLabelScript()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

const std::string & GetScoreLabelScript::getType() const
{
	return Type;
}

void GetScoreLabelScript::vPostInit()
{
	auto label_underlying = m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::CurrentScoreIncreased, this, [this, label_underlying](BaseEventData *){
		auto exploded_stars_num = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getExplodedStarsNum());
		auto attained_score = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getScoreOfPreviousExplosion());

		label_underlying->setString(std::string("Exploded: ") + exploded_stars_num + std::string(" Score: ") + attained_score);
		label_underlying->setVisible(true);
	});

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::LevelSummaryLabelDisappeared, this, [label_underlying](BaseEventData*){
		label_underlying->setVisible(false);
	});
}

const std::string GetScoreLabelScript::Type = "GetScoreLabelScript";
