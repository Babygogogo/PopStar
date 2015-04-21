#include "StatusBar.h"
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

	std::unique_ptr<GameObject> createHighScoreLabel();
	std::unique_ptr<GameObject> createCurrentScoreLabel();
	std::unique_ptr<GameObject> createTargetScoreLabel();
	std::unique_ptr<GameObject> createLevelLabel();
};

StatusBar::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

std::unique_ptr<GameObject> StatusBar::impl::createHighScoreLabel()
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont(
			ChineseWord("highestScore") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getHistoryScore())->_string, "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 50);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::HighScoreValueUpdated, this,
		[label_underlying](Event*){
			label_underlying->setString(ChineseWord("highestScore") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getHistoryScore())->_string);
	});

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createCurrentScoreLabel()
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont(
			cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getCurScore())->_string, "Verdana - Bold", 50); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 150);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::CurrentScoreValueUpdated, this,
		[label_underlying](Event*){
			label_underlying->setString(cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getCurScore())->_string);
	});

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createTargetScoreLabel()
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont(
			ChineseWord("mubiao") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getTargetScore())->_string + ChineseWord("fen"), "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(400, visibleSize.height - 100);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::TargetScoreValueUpdated, this,
		[label_underlying](Event*){
			label_underlying->setString(ChineseWord("mubiao") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getTargetScore())->_string + ChineseWord("fen"));
	});

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createLevelLabel()
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont(
			ChineseWord("guanqia") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getCurrentLevel())->_string, "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(200, visibleSize.height - 100);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelValueUpdated, this,
		[label_underlying](Event*){
			label_underlying->setString(ChineseWord("guanqia") + cocos2d::String::createWithFormat("%d", GAMEDATA::getInstance()->getCurrentLevel())->_string);
	});

	return label_object;
}

StatusBar::StatusBar(GameObject *game_object) :Script("StatusBar", game_object), pimpl(new impl)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Node>();

	game_object->addChild(pimpl->createHighScoreLabel());
	game_object->addChild(pimpl->createCurrentScoreLabel());
	game_object->addChild(pimpl->createTargetScoreLabel());
	game_object->addChild(pimpl->createLevelLabel());
}

StatusBar::~StatusBar()
{
}
