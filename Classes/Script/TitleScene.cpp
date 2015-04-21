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

	//HACK: It seems that the cocos2d-x engine has a mysterious bug here.
	//	If you create Menu with its factory method (e.g. auto menu = Menu::create(some_item..., NULL),
	//or auto menu = Menu::create(); menu->addChild(some_item);)
	//then all's right with the world.
	//	BUT if you create the menu and buttons with the GameObject and its components,
	//you will get a runtime error when you click the button, in line no. 128 of CCMenuItem.cpp.
	//	The cause is that the member CCMenuItem::_scriptType is changed somehow mysteriously,
	//without the script engine being initialized.
	//	To correct this, I made a change to the code of the engine, in line no. 124 of CCMenuItem.cpp:
	//before:	if (kScriptTypeNone != _scriptType)
	//after:	if (kScriptTypeNone != _scriptType && ScriptEngineManager::getInstance()->getScriptEngine())
	//	This change should be reasonable, but the better solution is to understand why the CCMenuItem::_scriptType is changed.
	std::unique_ptr<GameObject> createTitleMenu();
	std::unique_ptr<GameObject> createStartButton();
	std::function<void(cocos2d::Ref*)> createStartButtonCallback();
};

std::unique_ptr<GameObject> TitleScene::impl::createTitleLayer()
{
	auto title_layer_object = GameObject::create(
		[](GameObject* object){object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>(); });

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

	auto menu_object = GameObject::create();
	auto menu_underlying = menu_object->addComponent<DisplayNode>()->initAs<cocos2d::Menu>();

	menu_object->addChild(createStartButton());
	menu_underlying->alignItemsVertically();
	menu_underlying->setPosition(visibleSize.width / 2, visibleSize.height / 2);

	return menu_object;
}

std::unique_ptr<GameObject> TitleScene::impl::createStartButton()
{
	auto button_object = GameObject::create();
	button_object->addComponent<DisplayNode>()->initAs<cocos2d::MenuItemImage>(
		[this](){return cocos2d::MenuItemImage::create("menu_start.png", "menu_start.png", createStartButtonCallback()); }
	);

	return button_object;
}

std::function<void(cocos2d::Ref*)> TitleScene::impl::createStartButtonCallback()
{
	return [](cocos2d::Ref *sender){
		GAMEDATA::getInstance()->init();

		auto puzzle_scene = GameObject::create<PuzzleScene>();
		SingletonContainer::instance()->get<SceneStack>()->replaceAndRun(std::move(puzzle_scene));
	};
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
