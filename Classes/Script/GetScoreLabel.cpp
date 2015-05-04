#include "GetScoreLabel.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Common/SingletonContainer.h"
#include "../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"

struct GetScoreLabel::impl
{
	impl(GameObject *game_object);
	~impl();

	std::string createGetScoreText() const;
	void registerAsEventListeners(cocos2d::Label *label_underlying);
	void unregisterAsEventListeners();
};

GetScoreLabel::impl::impl(GameObject *game_object)
{
	game_object->setNeedUpdate(false);
	auto label_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
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
	auto exploded_stars_num = std::to_string(SingletonContainer::instance()->get<GameData>()->getExplodedStarsNum());
	auto attained_score = std::to_string(SingletonContainer::instance()->get<GameData>()->getScoreOfPreviousExplosion());

	return std::string("Exploded: ") + exploded_stars_num + std::string(" Score: ") + attained_score;
}

void GetScoreLabel::impl::registerAsEventListeners(cocos2d::Label *label_underlying)
{
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::CurrentScoreIncreased, this, [this, label_underlying](Event *){
		label_underlying->setString(createGetScoreText());
		label_underlying->setVisible(true);
	});

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelResultEnded, this, [label_underlying](Event*){
		label_underlying->setVisible(false);
	});
}

void GetScoreLabel::impl::unregisterAsEventListeners()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

GetScoreLabel::GetScoreLabel(GameObject *game_object) :Script("GetScoreLabel", game_object), pimpl(new impl(game_object))
{

}

GetScoreLabel::~GetScoreLabel()
{

}
