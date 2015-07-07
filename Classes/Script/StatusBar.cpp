#include "StatusBar.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Engine/Utilities/SingletonContainer.h"
#include "../Common/GameData.h"
#include "../Engine/Event/EventDispatcher.h"
#include "../Engine/Event/Event.h"
#include "../Engine/Event/EventType.h"

struct StatusBar::impl
{
	impl(GameObject *game_object);
	~impl();

	std::unique_ptr<GameObject> createHighScoreLabel();
	std::unique_ptr<GameObject> createCurrentScoreLabel();
	std::unique_ptr<GameObject> createTargetScoreLabel();
	std::unique_ptr<GameObject> createLevelLabel();
};

StatusBar::impl::impl(GameObject *game_object)
{
	game_object->setNeedUpdate(false);
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Node>();

	game_object->addChild(createHighScoreLabel());
	game_object->addChild(createCurrentScoreLabel());
	game_object->addChild(createTargetScoreLabel());
	game_object->addChild(createLevelLabel());
}

StatusBar::impl::~impl()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

std::unique_ptr<GameObject> StatusBar::impl::createHighScoreLabel()
{
	auto label_object = GameObject::create("HighScoreLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("High Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getHighScore()), "Verdana - Bold", 30); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width / 2, visible_size.height - 50);

	SingletonContainer::getInstance()->get<EventDispatcher>()->registerListener(LegacyEventType::HighScoreValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("High Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getHighScore())); });

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createCurrentScoreLabel()
{
	auto label_object = GameObject::create("CurrentScoreLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Current Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentScore()), "Verdana - Bold", 40); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width / 2, visible_size.height - 150);

	SingletonContainer::getInstance()->get<EventDispatcher>()->registerListener(LegacyEventType::CurrentScoreValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("Current Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentScore())); });

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createTargetScoreLabel()
{
	auto label_object = GameObject::create("TargetScoreLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Target Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getTargetScore()), "Verdana - Bold", 30); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(325, visible_size.height - 100);

	SingletonContainer::getInstance()->get<EventDispatcher>()->registerListener(LegacyEventType::TargetScoreValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("Target Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getTargetScore())); });

	return label_object;
}

std::unique_ptr<GameObject> StatusBar::impl::createLevelLabel()
{
	auto label_object = GameObject::create("LevelLabel");
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Level: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel()), "Verdana - Bold", 30); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(75, visible_size.height - 100);

	SingletonContainer::getInstance()->get<EventDispatcher>()->registerListener(LegacyEventType::LevelValueUpdated, this, [label_underlying](Event*){
		label_underlying->setString(std::string("Level: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel())); });

	return label_object;
}

StatusBar::StatusBar(GameObject *game_object) :Script("StatusBar", game_object), pimpl(new impl(game_object))
{

}

StatusBar::~StatusBar()
{
}
