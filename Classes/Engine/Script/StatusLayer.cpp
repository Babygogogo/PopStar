#include "StatusLayer.h"
#include "StatusBar.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
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
	game_object->addComponent<GeneralRenderComponent>()->initAs<cocos2d::Layer>();

	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();

	auto statusBarActor = std::make_shared<Actor>();
	statusBarActor->addComponent<StatusBar>();
	game_object->addChild(std::move(statusBarActor));

	game_object->addChild(gameLogic->createActor("Actors\\StartLevelLabel.xml"));
	game_object->addChild(gameLogic->createActor("Actors\\GetScoreLabel.xml"));
	game_object->addChild(gameLogic->createActor("Actors\\LevelSummaryLabel.xml"));
	game_object->addChild(gameLogic->createActor("Actors\\GameOverLabel.xml"));
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

const std::string & StatusLayer::getType() const
{
	return Type;
}

const std::string StatusLayer::Type = "StatusLayerScript";