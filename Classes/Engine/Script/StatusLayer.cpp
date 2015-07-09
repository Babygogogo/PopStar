#include "StatusLayer.h"
#include "StatusBar.h"
#include "GetScoreLabel.h"
#include "StartLevelLabel.h"
#include "LevelSummaryLabel.h"
#include "GameOverLabel.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"

struct StatusLayer::impl
{
	impl(Actor *game_object);
	~impl();
};

StatusLayer::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	game_object->addChild(Actor::create<StatusBar>("StatusBar"));
	game_object->addChild(Actor::create<StartLevelLabel>("StartLevelLabel"));
	game_object->addChild(Actor::create<GetScoreLabel>("GetScoreLabel"));
	game_object->addChild(Actor::create<LevelSummaryLabel>("LevelSummaryLabel"));
	game_object->addChild(Actor::create<GameOverLabel>("GameOverLabel"));
}

StatusLayer::impl::~impl()
{

}

StatusLayer::StatusLayer(Actor *game_object) :Script("StatusLayer", game_object), pimpl(new impl(game_object))
{

}

StatusLayer::~StatusLayer()
{

}

