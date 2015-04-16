#include "TitleScene.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Classes/Audio.h"
#include "../Classes/MenuLayer.h"

void addDependentComponents(GameObject *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<DisplayNode::Scene>()->addChild(MenuLayer::create());
}

TitleScene::TitleScene(GameObject* game_object) :Script("TitleScene", game_object)
{
	addDependentComponents(game_object);

	Audio::getInstance()->playBGM();
}

TitleScene::~TitleScene()
{

}
