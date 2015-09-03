#include "cocos2d.h"

#include "LevelSummaryLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataLevelSummaryStarted.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of LevelSummaryLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct LevelSummaryLabelScript::LevelSummaryLabelScriptImpl
{
	LevelSummaryLabelScriptImpl();
	~LevelSummaryLabelScriptImpl();

	void onLevelSummaryStarted(const IEventData & e);

	cocos2d::Label *m_label_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
};

LevelSummaryLabelScript::LevelSummaryLabelScriptImpl::LevelSummaryLabelScriptImpl()
{
}

LevelSummaryLabelScript::LevelSummaryLabelScriptImpl::~LevelSummaryLabelScriptImpl()
{
}

void LevelSummaryLabelScript::LevelSummaryLabelScriptImpl::onLevelSummaryStarted(const IEventData & e)
{
	//Set the summary text.
	const auto & levelSummaryEvent = static_cast<const EvtDataLevelSummaryStarted &>(e);
	auto stars_num_text = std::string("Stars Left: ") + std::to_string(levelSummaryEvent.getLeftStarsCount()) + '\n';
	auto bonus_text = std::string("Bonus: ") + std::to_string(levelSummaryEvent.getBonusScore());
	m_label_underlying->setString(stars_num_text + bonus_text);

	m_label_underlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_label_underlying->setPosition(visible_size.width + m_label_underlying->getContentSize().width / 2, visible_size.height / 2);

	//Reset the invoker, then invoke it.
	m_invoker->addMoveTo(0.6f, visible_size.width / 2, visible_size.height / 2);
	m_invoker->addMoveTo(0.6f, -m_label_underlying->getContentSize().width / 2, visible_size.height / 2, [this]{
		m_label_underlying->setVisible(false);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::LevelSummaryFinished));
	});
	m_invoker->invoke();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of LevelSummaryLabel.
//////////////////////////////////////////////////////////////////////////
LevelSummaryLabelScript::LevelSummaryLabelScript() : pimpl{ std::make_shared<LevelSummaryLabelScriptImpl>() }
{
}

LevelSummaryLabelScript::~LevelSummaryLabelScript()
{
}

void LevelSummaryLabelScript::vPostInit()
{
	auto actor = m_Actor.lock();

	pimpl->m_label_underlying = static_cast<cocos2d::Label*>(actor->getRenderComponent()->getSceneNode());
	pimpl->m_label_underlying->setVisible(false);

	pimpl->m_invoker = actor->getComponent<SequentialInvoker>().get();

	//Register as EventListener.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::LevelSummaryStarted, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryStarted(e);
	});

	//Register as touch listener.
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		pimpl->m_invoker->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, pimpl->m_label_underlying);
}

const std::string & LevelSummaryLabelScript::getType() const
{
	return Type;
}

const std::string LevelSummaryLabelScript::Type = "LevelSummaryLabelScript";
