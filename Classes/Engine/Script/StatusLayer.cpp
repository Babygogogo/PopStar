#include "StatusLayer.h"
#include "StatusBar.h"
#include "GetScoreLabel.h"
#include "StartLevelLabel.h"
#include "LevelSummaryLabel.h"
#include "GameOverLabel.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "cocos2d.h"

struct StatusLayer::impl
{
	impl(Actor *game_object);
	~impl();
};

StatusLayer::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	game_object->addChild(Actor::create<StatusBar>());
	game_object->addChild(Actor::create<StartLevelLabel>());
	game_object->addChild(Actor::create<GetScoreLabel>());
	game_object->addChild(Actor::create<LevelSummaryLabel>());
	game_object->addChild(Actor::create<GameOverLabel>());
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

