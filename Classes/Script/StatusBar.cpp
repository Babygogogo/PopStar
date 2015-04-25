#include "StatusBar.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Common/SingletonContainer.h"
#include "../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/Event.h"
#include "../Event/EventType.h"

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
	auto label_object = GameObject::create("HighScoreLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("High Score: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getHighScore()), "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 50);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::HighScoreValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("High Score: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getHighScore())); });

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createCurrentScoreLabel()
{
	auto label_object = GameObject::create("CurrentScoreLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Current Score: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getCurrentScore()), "Verdana - Bold", 40); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 150);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::CurrentScoreValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("Current Score: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getCurrentScore())); });

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createTargetScoreLabel()
{
	auto label_object = GameObject::create("TargetScoreLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Target Score: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getTargetScore()), "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(325, visibleSize.height - 100);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::TargetScoreValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("Target Score: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getTargetScore())); });

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createLevelLabel()
{
	auto label_object = GameObject::create("LevelLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Level: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getCurrentLevel()), "Verdana - Bold", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(75, visibleSize.height - 100);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("Level: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getCurrentLevel())); });

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
