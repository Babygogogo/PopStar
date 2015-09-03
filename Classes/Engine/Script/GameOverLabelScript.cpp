#include "cocos2d.h"

#include "GameOverLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../GameLogic/GameLogic.h"
#include "../Graphic2D/SceneStack.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameOverLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct GameOverLabelScript::GameOverLabelScriptImpl
{
	GameOverLabelScriptImpl();
	~GameOverLabelScriptImpl();

	void registerAsEventListeners();

	void onGameOver();
	void resetInvoker();

	cocos2d::Label *m_LabelUnderlying{ nullptr };
	std::weak_ptr<SequentialInvoker> m_Invoker;
};

GameOverLabelScript::GameOverLabelScriptImpl::GameOverLabelScriptImpl()
{
}

GameOverLabelScript::GameOverLabelScriptImpl::~GameOverLabelScriptImpl()
{
}

void GameOverLabelScript::GameOverLabelScriptImpl::registerAsEventListeners()
{
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		m_Invoker.lock()->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, m_LabelUnderlying);
}

void GameOverLabelScript::GameOverLabelScriptImpl::onGameOver()
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
GameOverLabelScript::GameOverLabelScript() : pimpl{ std::make_shared<GameOverLabelScriptImpl>() }
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::GameOver, pimpl, [this](const IEventData &){
		pimpl->onGameOver();
	});
}

GameOverLabelScript::~GameOverLabelScript()
{
}

bool GameOverLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void GameOverLabelScript::vPostInit()
{
	assert(!m_Actor.expired());
	auto strongActor = m_Actor.lock();

	pimpl->m_LabelUnderlying = static_cast<cocos2d::Label*>(strongActor->getRenderComponent()->getSceneNode());
	pimpl->m_LabelUnderlying->setVisible(false);

	pimpl->m_Invoker = strongActor->getComponent<SequentialInvoker>();

	pimpl->registerAsEventListeners();
}

const std::string & GameOverLabelScript::getType() const
{
	return Type;
}

const std::string GameOverLabelScript::Type = "GameOverLabelScript";
