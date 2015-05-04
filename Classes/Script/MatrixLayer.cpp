#include "MatrixLayer.h"
#include "TitleScene.h"
#include "StarMatrix.h"
#include "../Common/SingletonContainer.h"
#include "../Common/SceneStack.h"
#include "../Common/GameData.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/SequentialInvoker.h"
#include "../Event/Event.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventArg1.h"

#include "./Classes/Audio.h"

struct MatrixLayer::impl
{
	impl(GameObject *game_object);
	~impl();

	void resetTouchListenerForInvoker(GameObject *game_object);

	void addEventListeners();

	void startLevel();
	void onGameOver();

	GameObject *m_layer_object{ nullptr };
	GameObject *m_matrix{ nullptr };
};

MatrixLayer::impl::impl(GameObject *game_object) :m_layer_object(game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	m_matrix = game_object->addChild(GameObject::create<StarMatrix>("StarMatrix"));

	addEventListeners();
}

void MatrixLayer::impl::onGameOver()
{
	auto game_over_label = GameObject::create("GameOverLabel", [](GameObject *obj){obj->setNeedUpdate(false); });

	auto label_underlying = game_over_label->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{
		return cocos2d::Label::createWithSystemFont("Game Over", "Verdana-Bold", 80); });
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	auto label_size = label_underlying->getContentSize();
	label_underlying->setPosition(visible_size.width / 2, visible_size.height + label_size.height / 2);

	auto invoker = game_over_label->addComponent<SequentialInvoker>();
	invoker->addMoveTo(1, visible_size.width / 2, visible_size.height / 2);
	invoker->addCallback([]{SingletonContainer::instance()->get<SceneStack>()->replaceAndRun(GameObject::create<TitleScene>("TitleScene")); });
	invoker->invoke();
	resetTouchListenerForInvoker(game_over_label.get());

	m_layer_object->addChild(std::move(game_over_label));
}

void MatrixLayer::impl::addEventListeners()
{
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::StartLevelLabelDisappeared, this,
		[this](Event *){startLevel(); });
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::GameOver, this,
		[this](Event *){onGameOver(); });
}

void MatrixLayer::impl::resetTouchListenerForInvoker(GameObject *game_object)
{
	auto invoker = game_object->getComponent<SequentialInvoker>();
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [invoker](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		invoker->invoke();
		return true; };

	auto node_underlying = game_object->getComponent<DisplayNode>()->getAs<cocos2d::Node>();
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, node_underlying);
}

MatrixLayer::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void MatrixLayer::impl::startLevel()
{
	m_matrix->getComponent<StarMatrix>()->reset();
	Audio::getInstance()->playReadyGo();
}

MatrixLayer::MatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl(game_object))
{

}

MatrixLayer::~MatrixLayer()
{

}
