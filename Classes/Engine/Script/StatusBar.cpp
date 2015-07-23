#include "StatusBar.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/BaseEventData.h"
#include "../Event/EventType.h"
#include "cocos2d.h"

struct StatusBar::impl
{
	impl(Actor *game_object);
	~impl();

	std::shared_ptr<Actor> createHighScoreLabel();
	std::shared_ptr<Actor> createCurrentScoreLabel();
	std::shared_ptr<Actor> createTargetScoreLabel();
	std::shared_ptr<Actor> createLevelLabel();
};

StatusBar::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Node>();

	game_object->addChild(createHighScoreLabel());
	game_object->addChild(createCurrentScoreLabel());
	game_object->addChild(createTargetScoreLabel());
	game_object->addChild(createLevelLabel());
}

StatusBar::impl::~impl()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

std::shared_ptr<Actor> StatusBar::impl::createHighScoreLabel()
{
	auto label_object = std::make_shared<Actor>();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("High Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getHighScore()), "Verdana - Bold", 30); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width / 2, visible_size.height - 50);

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::HighScoreValueUpdated, this, [label_underlying](BaseEventData*){
		label_underlying->setString(std::string("High Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getHighScore())); });

	return label_object;
}

std::shared_ptr<Actor> StatusBar::impl::createCurrentScoreLabel()
{
	auto label_object = std::make_shared<Actor>();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Current Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentScore()), "Verdana - Bold", 40); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width / 2, visible_size.height - 150);

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::CurrentScoreValueUpdated, this, [label_underlying](BaseEventData*){
		label_underlying->setString(std::string("Current Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentScore())); });

	return label_object;
}

std::shared_ptr<Actor> StatusBar::impl::createTargetScoreLabel()
{
	auto label_object = std::make_shared<Actor>();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Target Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getTargetScore()), "Verdana - Bold", 30); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(325, visible_size.height - 100);

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::TargetScoreValueUpdated, this, [label_underlying](BaseEventData*){
		label_underlying->setString(std::string("Target Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getTargetScore())); });

	return label_object;
}

std::shared_ptr<Actor> StatusBar::impl::createLevelLabel()
{
	auto label_object = std::make_shared<Actor>();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont(
		std::string("Level: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel()), "Verdana - Bold", 30); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(75, visible_size.height - 100);

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::LevelValueUpdated, this, [label_underlying](BaseEventData*){
		label_underlying->setString(std::string("Level: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel())); });

	return label_object;
}

StatusBar::StatusBar(Actor *game_object) :BaseScriptComponent("StatusBar", game_object), pimpl(new impl(game_object))
{
}

StatusBar::~StatusBar()
{
}

const std::string & StatusBar::getType() const
{
	return Type;
}

const std::string StatusBar::Type = "StatusBarScript";
