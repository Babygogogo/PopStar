#include "TitleScene.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Classes/Audio.h"
#include "../Classes/MenuLayer.h"

TitleScene::TitleScene(GameObject* game_object) :Script("TitleScene", game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<DisplayNode::Scene>()->addChild(MenuLayer::create());

	Audio::getInstance()->playBGM();
}

TitleScene::~TitleScene()
{

}
