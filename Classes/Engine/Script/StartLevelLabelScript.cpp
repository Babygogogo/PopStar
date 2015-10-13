#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "StartLevelLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataLevelStarted.h"
#include "../Utilities/RelativePosition.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StartLevelLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StartLevelLabelScript::StartLevelLabelScriptImpl
{
	StartLevelLabelScriptImpl();
	~StartLevelLabelScriptImpl();

	void onLevelStarted(const IEventData & e);
	std::function<void()> _callbackOnMoveInFinished() const;

	static std::string s_LevelString;
	static std::string s_StartString;
	static RelativePosition s_InitialPosition;
	static RelativePosition s_MoveInFinishPosition;
	static RelativePosition s_MoveOutFinishPosition;
	static float s_MoveInDuration;
	static float s_MoveOutDuration;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<FiniteTimeActionComponent> m_ActionComponent;
};

std::string StartLevelLabelScript::StartLevelLabelScriptImpl::s_LevelString;
std::string StartLevelLabelScript::StartLevelLabelScriptImpl::s_StartString;
RelativePosition StartLevelLabelScript::StartLevelLabelScriptImpl::s_InitialPosition;
RelativePosition StartLevelLabelScript::StartLevelLabelScriptImpl::s_MoveInFinishPosition;
RelativePosition StartLevelLabelScript::StartLevelLabelScriptImpl::s_MoveOutFinishPosition;
float StartLevelLabelScript::StartLevelLabelScriptImpl::s_MoveInDuration{};
float StartLevelLabelScript::StartLevelLabelScriptImpl::s_MoveOutDuration{};

StartLevelLabelScript::StartLevelLabelScriptImpl::StartLevelLabelScriptImpl()
{
}

StartLevelLabelScript::StartLevelLabelScriptImpl::~StartLevelLabelScriptImpl()
{
}

void StartLevelLabelScript::StartLevelLabelScriptImpl::onLevelStarted(const IEventData & e)
{
	//Set the text and position of the label.
	const auto & levelStartedEvent = static_cast<const EvtDataLevelStarted &>(e);
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_RenderComponent.lock()->getSceneNode());
	underlyingLabel->setString(s_LevelString + std::to_string(levelStartedEvent.getLevelIndex()) + '\n' + s_StartString);
	underlyingLabel->setPosition(s_InitialPosition.getAbsolutePositionX(underlyingLabel), s_InitialPosition.getAbsolutePositionY(underlyingLabel));
	underlyingLabel->setVisible(true);

	//Clear all actions and move in the label with a callback.
	auto actionComponent = m_ActionComponent.lock();
	actionComponent->stopAndClearAllActions();
	actionComponent->queueMoveTo(s_MoveInDuration, s_MoveInFinishPosition.getAbsolutePositionX(underlyingLabel), s_MoveInFinishPosition.getAbsolutePositionY(underlyingLabel), _callbackOnMoveInFinished());
	actionComponent->runNextAction();
}

std::function<void()> StartLevelLabelScript::StartLevelLabelScriptImpl::_callbackOnMoveInFinished() const
{
	auto renderComponent = m_RenderComponent;
	auto actionComponent = m_ActionComponent;

	return [renderComponent, actionComponent](){
		//After the label finished moving in, we should be able to respond to player's touch.
		//To do this, create an touch event listener and add callback to it.
		auto touchListener = cocos2d::EventListenerTouchOneByOne::create();
		touchListener->onTouchBegan = [renderComponent, actionComponent](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
			//When touched, unregister as touch listener.
			auto underlyingNode = renderComponent.lock()->getSceneNode();
			cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(underlyingNode);

			//Then move out the label.
			auto strongActionComponent = actionComponent.lock();
			strongActionComponent->queueMoveTo(s_MoveOutDuration, s_MoveOutFinishPosition.getAbsolutePositionX(underlyingNode), s_MoveOutFinishPosition.getAbsolutePositionY(underlyingNode), [renderComponent]{
				//When the label finishes moving out, make it disappear and dispatch an event.
				renderComponent.lock()->getSceneNode()->setVisible(false);
				SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::StartLevelLabelDisappeared));
			});
			strongActionComponent->runNextAction();

			return true;
		};

		//Register the touch listener to event dispatcher.
		cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, renderComponent.lock()->getSceneNode());
	};
}

//////////////////////////////////////////////////////////////////////////
//Implementation of StartLevelLabelScript.
//////////////////////////////////////////////////////////////////////////
StartLevelLabelScript::StartLevelLabelScript() : pimpl{ std::make_shared<StartLevelLabelScriptImpl>() }
{
}

StartLevelLabelScript::~StartLevelLabelScript()
{
}

bool StartLevelLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto textElement = xmlElement->FirstChildElement("Text");
	pimpl->s_LevelString = textElement->Attribute("Level");
	pimpl->s_StartString = textElement->Attribute("Start");

	pimpl->s_InitialPosition.initialize(xmlElement->FirstChildElement("InitialPosition"));

	auto moveInElement = xmlElement->FirstChildElement("MoveIn");
	pimpl->s_MoveInDuration = moveInElement->FloatAttribute("Duration");
	pimpl->s_MoveInFinishPosition.initialize(moveInElement);

	auto moveOutElement = xmlElement->FirstChildElement("MoveOut");
	pimpl->s_MoveOutDuration = moveOutElement->FloatAttribute("Duration");
	pimpl->s_MoveOutFinishPosition.initialize(moveOutElement);

	isStaticInitialized = true;
	return true;
}

void StartLevelLabelScript::vPostInit()
{
	auto actor = m_Actor.lock();
	pimpl->m_RenderComponent = actor->getRenderComponent();
	pimpl->m_ActionComponent = actor->getComponent<FiniteTimeActionComponent>();

	//Register as EventListener.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::LevelStarted, pimpl, [this](const IEventData & e){
		pimpl->onLevelStarted(e);
	});
}

const std::string & StartLevelLabelScript::getType() const
{
	return Type;
}

const std::string StartLevelLabelScript::Type = "StartLevelLabelScript";
