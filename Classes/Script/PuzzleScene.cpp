#include "PuzzleScene.h"
#include "PuzzleMatrixLayer.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Classes/Audio.h"

PuzzleScene::PuzzleScene(GameObject* game_object) :Script("PuzzleScene", game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Scene>();
	game_object->addChild(GameObject::create<PuzzleMatrixLayer>());

	Audio::getInstance()->playBGM();
}

PuzzleScene::~PuzzleScene()
{

}
