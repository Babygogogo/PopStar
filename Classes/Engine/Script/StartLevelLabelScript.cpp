#include "cocos2d.h"

#include "StartLevelLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataLevelStarted.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StartLevelLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StartLevelLabelScript::StartLevelLabelScriptImpl
{
	StartLevelLabelScriptImpl();
	~StartLevelLabelScriptImpl();

	void onLevelStarted(const IEventData & e);

	cocos2d::Label *m_label_underlying{ nullptr };
	FiniteTimeActionComponent *m_invoker{ nullptr };
};

StartLevelLabelScript::StartLevelLabelScriptImpl::StartLevelLabelScriptImpl()
{
}

StartLevelLabelScript::StartLevelLabelScriptImpl::~StartLevelLabelScriptImpl()
{
}

void StartLevelLabelScript::StartLevelLabelScriptImpl::onLevelStarted(const IEventData & e)
{
	//Set the text of the label.
	const auto & levelStartedEvent = static_cast<const EvtDataLevelStarted &>(e);
	auto level_text = std::to_string(levelStartedEvent.getLevelIndex()) + '\n';
	m_label_underlying->setString(std::string("Level: ") + std::move(level_text) + std::string(" Start!"));
	m_label_underlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_label_underlying->setPosition(visible_size.width + m_label_underlying->getContentSize().width / 2, visible_size.height / 2);

	//Reset the invoker.
	m_invoker->queueMoveTo(0.6f, visible_size.width / 2, visible_size.height / 2);
	m_invoker->queueMoveTo(0.6f, -m_label_underlying->getContentSize().width / 2, visible_size.height / 2, [this]{
		m_label_underlying->setVisible(false);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::StartLevelLabelDisappeared));
	});
	m_invoker->runNextAction();
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

const std::string & StartLevelLabelScript::getType() const
{
	return Type;
}

void StartLevelLabelScript::vPostInit()
{
	auto actor = m_Actor.lock();

	pimpl->m_label_underlying = static_cast<cocos2d::Label*>(actor->getRenderComponent()->getSceneNode());
	pimpl->m_label_underlying->setVisible(false);

	pimpl->m_invoker = actor->getComponent<FiniteTimeActionComponent>().get();

	//Register as EventListener.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::LevelStarted, pimpl, [this](const IEventData & e){
		pimpl->onLevelStarted(e);
	});

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		pimpl->m_invoker->runNextAction();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, pimpl->m_label_underlying);
}

const std::string StartLevelLabelScript::Type = "StartLevelLabelScript";
