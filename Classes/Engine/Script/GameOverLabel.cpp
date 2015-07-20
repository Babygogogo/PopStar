#include "GameOverLabel.h"
#include "TitleScene.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../Graphic2D/SceneStack.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "cocos2d.h"

struct GameOverLabel::impl
{
	impl(Actor *game_object);
	~impl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void reset();
	void resetInvoker();

	std::string createGameOverText() const;

	cocos2d::Label *m_label_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
};

GameOverLabel::impl::impl(Actor *game_object)
{
	game_object->setNeedUpdate(false);

	m_label_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont("Game Over", "Verdana-Bold", 80); });
	m_label_underlying->setVisible(false);

	m_invoker = game_object->addComponent<SequentialInvoker>();

	registerAsEventListeners();
}

GameOverLabel::impl::~impl()
{
	unregisterAsEventListeners();
}

void GameOverLabel::impl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::GameOver, this,
		[this](BaseEventData *){reset(); });

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		m_invoker->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, m_label_underlying);
}

void GameOverLabel::impl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void GameOverLabel::impl::reset()
{
	m_label_underlying->setString(createGameOverText());
	m_label_underlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	auto label_size = m_label_underlying->getContentSize();
	m_label_underlying->setPosition(visible_size.width / 2, visible_size.height + label_size.height / 2);

	resetInvoker();
	m_invoker->invoke();
}

void GameOverLabel::impl::resetInvoker()
{
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_invoker->addMoveTo(1, visible_size.width / 2, visible_size.height / 2);
	m_invoker->addCallback([]{SingletonContainer::getInstance()->get<SceneStack>()->replaceAndRun(Actor::create<TitleScene>()); });
}

std::string GameOverLabel::impl::createGameOverText() const
{
	return "Game Over";
}

GameOverLabel::GameOverLabel(Actor *game_object) :BaseScriptComponent("GameOverLabel", game_object), pimpl(new impl(game_object))
{

}

GameOverLabel::~GameOverLabel()
{

}
