#include "PuzzleScene.h"
#include "StatusLayer.h"
#include "MatrixLayer.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Audio/Audio.h"
#include "cocos2d.h"

struct PuzzleScene::impl
{
	impl(Actor *game_object);
	~impl();

	std::unique_ptr<Actor> createBackground();
};

PuzzleScene::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Scene>();

	game_object->addChild(createBackground());
	game_object->addChild(Actor::create<MatrixLayer>("MatrixLayer"));
	game_object->addChild(Actor::create<StatusLayer>("PuzzleStatusLayer"));

	Audio::getInstance()->playBGM();
}

PuzzleScene::impl::~impl()
{

}

std::unique_ptr<Actor> PuzzleScene::impl::createBackground()
{
	auto background_object = Actor::create("PuzzleBackgroundSprite");
	auto background_sprite = background_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>([]{return cocos2d::Sprite::create("bg_mainscene.jpg"); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	background_sprite->setPosition(visible_size.width / 2, visible_size.height / 2);
	background_sprite->setLocalZOrder(-1);

	return background_object;
}

PuzzleScene::PuzzleScene(Actor* game_object) :BaseScriptComponent("PuzzleScene", game_object), pimpl(new impl(game_object))
{

}

PuzzleScene::~PuzzleScene()
{

}
