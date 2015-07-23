#include "StatusLayer.h"
#include "StatusBar.h"
#include "GetScoreLabel.h"
#include "StartLevelLabel.h"
#include "LevelSummaryLabel.h"
#include "GameOverLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Utilities/SingletonContainer.h"
#include "../GameLogic/GameLogic.h"
#include "cocos2d.h"

struct StatusLayer::impl
{
	impl(Actor *game_object);
	~impl();
};

StatusLayer::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	auto statusBarActor = std::make_shared<Actor>();
	statusBarActor->addComponent<StatusBar>();
	game_object->addChild(std::move(statusBarActor));

	auto startLevelLabelActor = std::make_shared<Actor>();
	startLevelLabelActor->addComponent<StartLevelLabel>();
	game_object->addChild(std::move(startLevelLabelActor));

	auto getScoreLabelActor = std::make_shared<Actor>();
	getScoreLabelActor->addComponent<GetScoreLabel>();
	game_object->addChild(std::move(getScoreLabelActor));

	auto levelSummaryLabelActor = std::make_shared<Actor>();
	levelSummaryLabelActor->addComponent<LevelSummaryLabel>();
	game_object->addChild(std::move(levelSummaryLabelActor));

	//auto gameOverLabelActor = std::make_shared<Actor>();
	//gameOverLabelActor->addComponent<GameOverLabelScript>();
	auto gameOverLabelActor = SingletonContainer::getInstance()->get<GameLogic>()->createActor("Actors\\GameOverLabel.xml");
	game_object->addChild(std::move(gameOverLabelActor));
}

StatusLayer::impl::~impl()
{
}

StatusLayer::StatusLayer(Actor *game_object) :BaseScriptComponent("StatusLayer", game_object), pimpl(new impl(game_object))
{
}

StatusLayer::~StatusLayer()
{
}
