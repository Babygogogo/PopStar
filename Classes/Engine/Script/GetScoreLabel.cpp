#include "GetScoreLabel.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "cocos2d.h"

struct GetScoreLabel::impl
{
	impl(Actor *game_object);
	~impl();

	std::string createGetScoreText() const;
	void registerAsEventListeners(cocos2d::Label *label_underlying);
	void unregisterAsEventListeners();
};

GetScoreLabel::impl::impl(Actor *game_object)
{
	auto label_underlying = game_object->addComponent<GeneralRenderComponent>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont("", "Arial", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 250);

	registerAsEventListeners(label_underlying);
}

GetScoreLabel::impl::~impl()
{
	unregisterAsEventListeners();
}

std::string GetScoreLabel::impl::createGetScoreText() const
{
	auto exploded_stars_num = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getExplodedStarsNum());
	auto attained_score = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getScoreOfPreviousExplosion());

	return std::string("Exploded: ") + exploded_stars_num + std::string(" Score: ") + attained_score;
}

void GetScoreLabel::impl::registerAsEventListeners(cocos2d::Label *label_underlying)
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::CurrentScoreIncreased, this, [this, label_underlying](BaseEventData *){
		label_underlying->setString(createGetScoreText());
		label_underlying->setVisible(true);
	});

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::LevelSummaryLabelDisappeared, this, [label_underlying](BaseEventData*){
		label_underlying->setVisible(false);
	});
}

void GetScoreLabel::impl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

GetScoreLabel::GetScoreLabel(Actor *game_object) :BaseScriptComponent("GetScoreLabel", game_object), pimpl(new impl(game_object))
{
}

GetScoreLabel::~GetScoreLabel()
{
}

const std::string & GetScoreLabel::getType() const
{
	return Type;
}

const std::string GetScoreLabel::Type = "GetScoreLabelScript";
