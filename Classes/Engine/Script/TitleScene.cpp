#include "TitleScene.h"
#include "PuzzleScene.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Utilities/SingletonContainer.h"
#include "../Graphic2D/SceneStack.h"
#include "../../Common/GameData.h"
#include "../Audio/Audio.h"

struct TitleScene::impl
{
	impl(Actor *game_object);
	~impl();

	std::unique_ptr<Actor> createDisplayObjects() const;

	std::unique_ptr<Actor> createBackground() const;
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
	std::unique_ptr<Actor> createTitleMenu() const;
	std::unique_ptr<Actor> createStartButton() const;
	std::function<void(cocos2d::Ref*)> createStartButtonCallback() const;
};

TitleScene::impl::impl(Actor *game_object)
{
	game_object->setNeedUpdate(false);
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Scene>();

	game_object->addChild(createDisplayObjects());

	Audio::getInstance()->playBGM();
}

TitleScene::impl::~impl()
{

}

std::unique_ptr<Actor> TitleScene::impl::createDisplayObjects() const
{
	auto layer_object = Actor::create("TitleLayer");
	layer_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	layer_object->addChild(createBackground());
	layer_object->addChild(createTitleMenu());

	return layer_object;
}

std::unique_ptr<Actor> TitleScene::impl::createBackground() const
{
	auto background_object = Actor::create("TitleBackgroundSprite");
	auto background_sprite = background_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>(
		[]{return cocos2d::Sprite::create("bg_menuscene.jpg"); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	background_sprite->setPosition(visible_size.width / 2, visible_size.height / 2);

	return background_object;
}

std::unique_ptr<Actor> TitleScene::impl::createTitleMenu() const
{
	auto menu_object = Actor::create("TitleMenu");
	auto menu_underlying = menu_object->addComponent<DisplayNode>()->initAs<cocos2d::Menu>();

	menu_object->addChild(createStartButton());

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	menu_underlying->setPosition(visible_size.width / 2, visible_size.height / 2);

	return menu_object;
}

std::unique_ptr<Actor> TitleScene::impl::createStartButton() const
{
	auto button_object = Actor::create("StartButton");
	button_object->addComponent<DisplayNode>()->initAs<cocos2d::MenuItemImage>(
		[this]{return cocos2d::MenuItemImage::create("menu_start.png", "menu_start.png", createStartButtonCallback()); });

	return button_object;
}

std::function<void(cocos2d::Ref*)> TitleScene::impl::createStartButtonCallback() const
{
	return [](cocos2d::Ref*){
		SingletonContainer::getInstance()->get<GameData>()->reset();
		SingletonContainer::getInstance()->get<SceneStack>()->replaceAndRun(Actor::create<PuzzleScene>("PuzzleScene"));
	};
}

TitleScene::TitleScene(Actor* game_object) :Script("TitleScene", game_object), pimpl(new impl(game_object))
{

}

TitleScene::~TitleScene()
{

}
