#include "GameOverLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../GameLogic/GameLogic.h"
#include "../Graphic2D/SceneStack.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameOverLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct GameOverLabelScript::GameOverLabelScriptImpl
{
	GameOverLabelScriptImpl();
	~GameOverLabelScriptImpl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void reset();
	void resetInvoker();

	cocos2d::Label *m_LabelUnderlying{ nullptr };
	std::weak_ptr<SequentialInvoker> m_Invoker;
};

GameOverLabelScript::GameOverLabelScriptImpl::GameOverLabelScriptImpl()
{
}

GameOverLabelScript::GameOverLabelScriptImpl::~GameOverLabelScriptImpl()
{
	unregisterAsEventListeners();
}

void GameOverLabelScript::GameOverLabelScriptImpl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::GameOver, this,
		[this](BaseEventData *){reset(); });

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		m_Invoker.lock()->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, m_LabelUnderlying);
}

void GameOverLabelScript::GameOverLabelScriptImpl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void GameOverLabelScript::GameOverLabelScriptImpl::reset()
{
	m_LabelUnderlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	auto label_size = m_LabelUnderlying->getContentSize();
	m_LabelUnderlying->setPosition(visible_size.width / 2, visible_size.height + label_size.height / 2);

	resetInvoker();
	m_Invoker.lock()->invoke();
}

void GameOverLabelScript::GameOverLabelScriptImpl::resetInvoker()
{
	auto strongInvoker = m_Invoker.lock();
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	strongInvoker->addMoveTo(1, visible_size.width / 2, visible_size.height / 2);

	strongInvoker->addCallback([]{
		auto & singletonContainer = SingletonContainer::getInstance();
		auto titleScene = singletonContainer->get<GameLogic>()->createActor("Actors\\TitleScene.xml");
		singletonContainer->get<SceneStack>()->replaceAndRun(*titleScene);
	});
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameOverLabelScript.
//////////////////////////////////////////////////////////////////////////
GameOverLabelScript::GameOverLabelScript() : pimpl{ std::make_unique<GameOverLabelScriptImpl>() }
{
}

GameOverLabelScript::~GameOverLabelScript()
{
}

const std::string & GameOverLabelScript::getType() const
{
	return Type;
}

bool GameOverLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void GameOverLabelScript::vPostInit()
{
	assert(!m_Actor.expired());
	auto strongActor = m_Actor.lock();

	pimpl->m_LabelUnderlying = strongActor->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
	pimpl->m_LabelUnderlying->setVisible(false);

	pimpl->m_Invoker = strongActor->getComponent<SequentialInvoker>();

	pimpl->registerAsEventListeners();
}

const std::string GameOverLabelScript::Type = "GameOverLabelScript";
