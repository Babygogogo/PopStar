#include "MenuLayer.h"
#include "GameData.h"
#include "Audio.h"

#include "./Common/SingletonContainer.h"
#include "./Common/SceneStack.h"
#include "./Script/PuzzleScene.h"

using namespace cocos2d;

bool MenuLayer::init(){
	if(!Layer::init()){
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	/*初始化背景*/
	Sprite* background = Sprite::create("bg_menuscene.jpg");
	background->setPosition(visibleSize.width/2,visibleSize.height/2);
	this->addChild(background,-1);
	
	/*初始化菜单*/
	MenuItemImage* startBtn = MenuItemImage::create(
		"menu_start.png","menu_star.png",CC_CALLBACK_0(MenuLayer::startGame,this)
		);
	Menu* menu = Menu::create(startBtn,NULL);
	menu->alignItemsVertically();
	menu->setPosition(visibleSize.width/2,visibleSize.height/2);
	this->addChild(menu);
	return true;
}

void MenuLayer::startGame(){
	CCLOG("START!");
	GAMEDATA::getInstance()->init();

	auto puzzle_scene = GameObject::create();
	puzzle_scene->addComponent<PuzzleScene>();
	SingletonContainer::instance().get<SceneStack>()->replaceAndRun(std::move(puzzle_scene));
}
