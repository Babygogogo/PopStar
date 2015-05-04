#include "StatusLayer.h"
#include "StatusBar.h"
#include "GetScoreLabel.h"
#include "StartLevelLabel.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"

struct StatusLayer::impl
{
	impl(GameObject *game_object);
	~impl();
};

StatusLayer::impl::impl(GameObject *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	game_object->addChild(GameObject::create<StatusBar>("StatusBar"));
	game_object->addChild(GameObject::create<StartLevelLabel>("StartLevelLabel"));
	game_object->addChild(GameObject::create<GetScoreLabel>("GetScoreLabel"));
}

StatusLayer::impl::~impl()
{

}

StatusLayer::StatusLayer(GameObject *game_object) :Script("StatusLayer", game_object), pimpl(new impl(game_object))
{

}

StatusLayer::~StatusLayer()
{

}

