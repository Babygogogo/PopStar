#include "TitleScene.h"
#include "PuzzleScene.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Common/SingletonContainer.h"
#include "../Common/SceneStack.h"
#include "../Classes/Audio.h"
#include "../Classes/GameData.h"

struct TitleScene::impl
{
	impl(){};
	~impl(){};

	std::unique_ptr<GameObject> createTitleLayer();

	std::unique_ptr<GameObject> createBackground();

	std::unique_ptr<GameObject> createTitleMenu();
	std::unique_ptr<GameObject> createStartButton();
};

std::unique_ptr<GameObject> TitleScene::impl::createTitleLayer()
{
	auto title_layer_object = GameObject::create();
	title_layer_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	title_layer_object->addChild(createBackground());
	title_layer_object->addChild(createTitleMenu());

	return title_layer_object;
}

std::unique_ptr<GameObject> TitleScene::impl::createBackground()
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	auto background_object = GameObject::create();
	auto background_sprite = background_object->addComponent<DisplayNode>()->
		initAs<cocos2d::Sprite>([](){return cocos2d::Sprite::create("bg_menuscene.jpg"); });
	background_sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	return background_object;
}

std::unique_ptr<GameObject> TitleScene::impl::createTitleMenu()
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	auto button_object = GameObject::create();
	auto button_sprite = button_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();

	auto startBtn = cocos2d::MenuItemImage::create(
		"menu_start.png",
		"menu_star.png",
		[](cocos2d::Ref *sender){
			GAMEDATA::getInstance()->init();
			
			auto puzzle_scene = GameObject::create();
			puzzle_scene->addComponent<PuzzleScene>();
			SingletonContainer::instance().get<SceneStack>()->replaceAndRun(std::move(puzzle_scene));
	});

	auto menu = cocos2d::Menu::create(startBtn, NULL);
	menu->alignItemsVertically();
	menu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	button_sprite->addChild(menu);

	return button_object;
}

std::unique_ptr<GameObject> TitleScene::impl::createStartButton()
{
	auto button_object = GameObject::create();
	button_object->addComponent<DisplayNode>()->initAs<cocos2d::MenuItemImage>([](){
		return cocos2d::MenuItemImage::create(
			"menu_start.png",
			"menu_star.png",
			[](cocos2d::Ref *sender){
				GAMEDATA::getInstance()->init();

				auto puzzle_scene = GameObject::create();
				puzzle_scene->addComponent<PuzzleScene>();
				SingletonContainer::instance().get<SceneStack>()->replaceAndRun(std::move(puzzle_scene));
		});
	});

	return button_object;
}

TitleScene::TitleScene(GameObject* game_object) :Script("TitleScene", game_object), pimpl(new impl)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Scene>();
	game_object->addChild(pimpl->createTitleLayer());

	Audio::getInstance()->playBGM();
}

TitleScene::~TitleScene()
{

}
