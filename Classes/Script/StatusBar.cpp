#include "StatusBar.h"
#include "Label.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Common/SingletonContainer.h"
#include "../Event/EventDispatcher.h"
#include "../Event/Event.h"
#include "../Event/EventType.h"

#include "../Classes/Chinese.h"
#include "../Classes/GameData.h"

struct StatusBar::impl
{
	impl(){};
	~impl();

	std::unique_ptr<GameObject> createHighScoreLabel(const std::string &text);
};

StatusBar::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

std::unique_ptr<GameObject> StatusBar::impl::createHighScoreLabel(const std::string &text)
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont(
			ChineseWord("highestScore") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getHistoryScore())->_string, "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 50);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::StarExploded, this,
		[label_underlying](Event*){
			label_underlying->setString(ChineseWord("highestScore") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getHistoryScore())->_string);
	});

	return label_object;
}

StatusBar::StatusBar(GameObject *game_object) :Script("StatusBar", game_object), pimpl(new impl)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Node>();
	game_object->addChild(pimpl->createHighScoreLabel("TestHighScore"));
}

StatusBar::~StatusBar()
{
}
