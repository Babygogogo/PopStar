#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "GameOverLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"
#include "../GameLogic/GameLogic.h"
#include "../Graphic2D/SceneStack.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameOverLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct GameOverLabelScript::GameOverLabelScriptImpl
{
	GameOverLabelScriptImpl();
	~GameOverLabelScriptImpl();

	void onGameOver();
	std::function<void()> _callbackOnLabelMoveFinished() const;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<FiniteTimeActionComponent> m_FiniteTimeActionComponent;
};

GameOverLabelScript::GameOverLabelScriptImpl::GameOverLabelScriptImpl()
{
}

GameOverLabelScript::GameOverLabelScriptImpl::~GameOverLabelScriptImpl()
{
}

void GameOverLabelScript::GameOverLabelScriptImpl::onGameOver()
{
	auto underlyingNode = m_RenderComponent.lock()->getSceneNode();
	underlyingNode->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	auto label_size = underlyingNode->getContentSize();
	underlyingNode->setPosition(visible_size.width / 2, visible_size.height + label_size.height / 2);

	auto actionComponent = m_FiniteTimeActionComponent.lock();
	actionComponent->stopAndClearAllActions();
	actionComponent->queueMoveTo(1, visible_size.width / 2, visible_size.height / 2, _callbackOnLabelMoveFinished());
	actionComponent->runNextAction();
}

std::function<void()> GameOverLabelScript::GameOverLabelScriptImpl::_callbackOnLabelMoveFinished() const
{
	auto renderComponent = m_RenderComponent;
	return [renderComponent](){
		//After the label finished moving, we should be able to respond to player's touch.
		//To do this, create an touch event listener and add callback to it.
		auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = [renderComponent](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
			//When touched, unregister as touch listener.
			cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(renderComponent.lock()->getSceneNode());

			//Then run the TitleScene.
			auto & singletonContainer = SingletonContainer::getInstance();
			auto titleScene = singletonContainer->get<GameLogic>()->createActor("Actors\\TitleScene.xml");
			singletonContainer->get<SceneStack>()->replaceAndRun(*titleScene);
			return true;
		};

		//Register the touch listener to event dispatcher.
		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, renderComponent.lock()->getSceneNode());
	};
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameOverLabelScript.
//////////////////////////////////////////////////////////////////////////
GameOverLabelScript::GameOverLabelScript() : pimpl{ std::make_shared<GameOverLabelScriptImpl>() }
{
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
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::GameOver, pimpl, [this](const IEventData &){
		pimpl->onGameOver();
	});

	auto strongActor = m_Actor.lock();
	auto renderComponent = strongActor->getRenderComponent();
	renderComponent->getSceneNode()->setVisible(false);

	pimpl->m_RenderComponent = renderComponent;
	pimpl->m_FiniteTimeActionComponent = strongActor->getComponent<FiniteTimeActionComponent>();
}

const std::string & GameOverLabelScript::getType() const
{
	return Type;
}

const std::string GameOverLabelScript::Type = "GameOverLabelScript";
