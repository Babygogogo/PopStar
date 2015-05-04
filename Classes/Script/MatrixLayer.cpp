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

	void createAndAttachLevelMessageLabel(GameObject *layer_object, GameObject *&label_object, std::string label_name);
	void resetLevelMessageLabel(GameObject *label_object, const std::string &text, std::function<void()> &&callback_after_disappear);
	void resetInvokerForLevelMessageLabel(GameObject *label_object, std::function<void()> &&callback_after_disappear);
	void resetTouchListenerForInvoker(GameObject *game_object);

	void addEventListeners();

	void startLevel();
	void showLevelSummary(int num_stars_left);
	void onGameOver();

	GameObject *m_layer_object{ nullptr };
	GameObject *m_summary_label{ nullptr };
	GameObject *m_matrix{ nullptr };
};

MatrixLayer::impl::impl(GameObject *game_object) :m_layer_object(game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	m_matrix = game_object->addChild(GameObject::create<StarMatrix>("StarMatrix"));

	createAndAttachLevelMessageLabel(game_object, m_summary_label, "PuzzleLevelSummaryLabel");

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
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelNoMoreMove, this,
		[this](Event *e){showLevelSummary(static_cast<EventArg1*>(e->getArg())->getInt()); });
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::GameOver, this,
		[this](Event *){onGameOver(); });
}

void MatrixLayer::impl::createAndAttachLevelMessageLabel(GameObject *layer_object, GameObject *&label_object_ref, std::string label_name)
{
	auto new_label_object = GameObject::create(std::move(label_name), [](GameObject* obj){obj->setNeedUpdate(false); });
	new_label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont("", "Verdana-Bold", 50); })->setLocalZOrder(1);
	new_label_object->addComponent<SequentialInvoker>();

	label_object_ref = new_label_object.get();
	layer_object->addChild(std::move(new_label_object));
}

void MatrixLayer::impl::resetLevelMessageLabel(GameObject *label_object, const std::string &text, std::function<void()> &&callback_after_disappear)
{
	auto label_underlying = label_object->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	label_underlying->setString(text);
	label_underlying->setVisible(true);

	resetInvokerForLevelMessageLabel(label_object, std::move(callback_after_disappear));
	resetTouchListenerForInvoker(label_object);
}

void MatrixLayer::impl::resetInvokerForLevelMessageLabel(GameObject *label_object, std::function<void()> &&callback_after_disappear)
{
	auto label_underlying = label_object->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width + label_underlying->getContentSize().width / 2, visible_size.height / 2);

	auto invoker = label_object->getComponent<SequentialInvoker>();
	invoker->addMoveTo(0.6f, visible_size.width / 2, visible_size.height / 2);
	invoker->addMoveTo(0.6f, -label_underlying->getContentSize().width / 2, visible_size.height / 2, std::move(callback_after_disappear));
	invoker->invoke();
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

void MatrixLayer::impl::showLevelSummary(int num_stars_left)
{
	auto label_underlying = m_summary_label->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	resetLevelMessageLabel(m_summary_label,
		std::string("Stars Left: ") + std::to_string(num_stars_left) + '\n' + std::string("Bonus: ") + std::to_string(SingletonContainer::instance()->get<GameData>()->getEndLevelBonus(num_stars_left)),
		[this, label_underlying, num_stars_left]{
			SingletonContainer::instance()->get<GameData>()->updateEndLevelScoreWith(num_stars_left);
			SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(Event::create(EventType::LevelResultEnded));
			label_underlying->setVisible(false);
			cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(label_underlying); });
}

MatrixLayer::MatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl(game_object))
{

}

MatrixLayer::~MatrixLayer()
{
}
