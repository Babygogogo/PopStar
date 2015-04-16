#include "PuzzleScene.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Classes/Audio.h"
#include "../Classes/GameLayer.h"

PuzzleScene::PuzzleScene(GameObject* game_object) :Script("PuzzleScene", game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<DisplayNode::Scene>()->addChild(GameLayer::create());

	Audio::getInstance()->playBGM();
}

PuzzleScene::~PuzzleScene()
{

}
