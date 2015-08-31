#include "StartLevelLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StartLevelLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StartLevelLabelScript::StartLevelLabelScriptImpl
{
	StartLevelLabelScriptImpl();
	~StartLevelLabelScriptImpl();

	void onLevelStarted();

	cocos2d::Label *m_label_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
};

StartLevelLabelScript::StartLevelLabelScriptImpl::StartLevelLabelScriptImpl()
{
}

StartLevelLabelScript::StartLevelLabelScriptImpl::~StartLevelLabelScriptImpl()
{
}

void StartLevelLabelScript::StartLevelLabelScriptImpl::onLevelStarted()
{
	//Set the text of the label.
	auto level_text = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel()) + '\n';
	m_label_underlying->setString(std::string("Level: ") + std::move(level_text) + std::string(" Start!"));
	m_label_underlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_label_underlying->setPosition(visible_size.width + m_label_underlying->getContentSize().width / 2, visible_size.height / 2);

	//Reset the invoker.
	m_invoker->addMoveTo(0.6f, visible_size.width / 2, visible_size.height / 2);
	m_invoker->addMoveTo(0.6f, -m_label_underlying->getContentSize().width / 2, visible_size.height / 2, [this]{
		m_label_underlying->setVisible(false);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::StartLevelLabelDisappeared));
	});
	m_invoker->invoke();
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

	pimpl->m_label_underlying = actor->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Label>();
	pimpl->m_label_underlying->setVisible(false);

	pimpl->m_invoker = actor->getComponent<SequentialInvoker>().get();

	//Register as EventListener.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::LevelStarted, pimpl, [this](const IEventData & e){
		pimpl->onLevelStarted();
	});

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		pimpl->m_invoker->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, pimpl->m_label_underlying);
}

const std::string StartLevelLabelScript::Type = "StartLevelLabelScript";
