#include <list>

#include "SequentialInvoker.h"
#include "Actor.h"
#include "GeneralRenderComponent.h"
#include "../Utilities/SingletonContainer.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SequentialInvokerImpl.
//////////////////////////////////////////////////////////////////////////
struct SequentialInvoker::SequentialInvokerImpl
{
	SequentialInvokerImpl(SequentialInvoker *visitor);
	SequentialInvokerImpl(cocos2d::Node *target_node);
	~SequentialInvokerImpl();

	void init();

	void pushBack(cocos2d::Action *action);
	void popFront();
	void eraseCurrent();

	bool invoke(bool is_called_by_event_dispatcher);
	bool isInvoking() const;

	cocos2d::CallFunc *createDispatchCallback() const;

	SequentialInvoker *m_Visitor{ nullptr };
	cocos2d::Node *m_TargetNode{ nullptr };
	cocos2d::Action *m_current_action{ nullptr };
	bool m_invoke_continuously{ false };
	std::list<cocos2d::Action*> m_action_list;
};

SequentialInvoker::SequentialInvokerImpl::SequentialInvokerImpl(SequentialInvoker *visitor) : m_Visitor{ visitor }
{
}

SequentialInvoker::SequentialInvokerImpl::SequentialInvokerImpl(cocos2d::Node *target_node) : m_TargetNode(target_node)
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(
		EventType::SequentialInvokerFinishOneAction, this, [this](BaseEventData *e){eraseCurrent(); invoke(true); });
}

SequentialInvoker::SequentialInvokerImpl::~SequentialInvokerImpl()
{
	while (!m_action_list.empty())
		popFront();

	eraseCurrent();

	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void SequentialInvoker::SequentialInvokerImpl::init()
{
	m_TargetNode = m_Visitor->m_Actor.lock()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Node>();
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(
		EventType::SequentialInvokerFinishOneAction, this, [this](BaseEventData *e){eraseCurrent(); invoke(true); });
}

void SequentialInvoker::SequentialInvokerImpl::popFront()
{
	if (m_action_list.empty())
		return;

	m_action_list.front()->release();
	m_action_list.pop_front();
}

void SequentialInvoker::SequentialInvokerImpl::pushBack(cocos2d::Action *action)
{
	action->retain();
	m_action_list.push_back(action);
}

void SequentialInvoker::SequentialInvokerImpl::eraseCurrent()
{
	if (!m_current_action)
		return;

	m_current_action->release();
	m_current_action = nullptr;
}

bool SequentialInvoker::SequentialInvokerImpl::invoke(bool is_called_by_event_dispatcher)
{
	if (is_called_by_event_dispatcher && !m_invoke_continuously)
		return false;
	if (isInvoking() || m_action_list.empty())
		return false;

	m_current_action = m_action_list.front();
	m_action_list.pop_front();
	m_TargetNode->runAction(m_current_action);

	return true;
}

bool SequentialInvoker::SequentialInvokerImpl::isInvoking() const
{
	if (!m_current_action)
		return false;

	return !m_current_action->isDone();
}

cocos2d::CallFunc * SequentialInvoker::SequentialInvokerImpl::createDispatchCallback() const
{
	return cocos2d::CallFunc::create([this]{if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->dispatch(
		std::make_unique<EvtDataGeneric>(EventType::SequentialInvokerFinishOneAction), const_cast<SequentialInvoker::SequentialInvokerImpl*>(this)); });
}

//////////////////////////////////////////////////////////////////////////
//Implementation of SequentialInvoker.
//////////////////////////////////////////////////////////////////////////
SequentialInvoker::SequentialInvoker() : pimpl{ std::make_unique<SequentialInvokerImpl>(this) }
{
}

SequentialInvoker::SequentialInvoker(Actor *game_object)
{
	auto target_node = game_object->addComponent<GeneralRenderComponent>()->getAs<cocos2d::Node>();
	if (!target_node)
		throw("Add SequentialInvoker to a GameObject without an initialized DisplayNode.");

	pimpl.reset(new SequentialInvokerImpl(target_node));
}

SequentialInvoker::~SequentialInvoker()
{
}

bool SequentialInvoker::isInvoking() const
{
	return pimpl->isInvoking();
}

bool SequentialInvoker::invoke()
{
	return pimpl->invoke(false);
}

void SequentialInvoker::addMoveTo(float duration_s, float x, float y, std::function<void()> &&callback /*= nullptr*/)
{
	auto move_to = cocos2d::MoveTo::create(duration_s, { x, y });
	auto step = callback ?
		cocos2d::Sequence::create(move_to, cocos2d::CallFunc::create(callback), nullptr) :
		cocos2d::Sequence::create(move_to, nullptr);

	addFiniteTimeAction(step);
}

void SequentialInvoker::addDelay(float delay_s)
{
	if (delay_s <= 0)
		throw("addDelay with negative time in SequentialInvoker.");

	addFiniteTimeAction(cocos2d::DelayTime::create(delay_s));
}

void SequentialInvoker::addCallback(std::function<void()> &&callback)
{
	addFiniteTimeAction(cocos2d::CallFunc::create(callback));
}

void SequentialInvoker::addFiniteTimeAction(cocos2d::FiniteTimeAction* action)
{
	pimpl->pushBack(cocos2d::Sequence::create(action, pimpl->createDispatchCallback(), nullptr));
	if (pimpl->m_invoke_continuously)
		invoke();
}

void SequentialInvoker::setInvokeContinuously(bool continuously)
{
	pimpl->m_invoke_continuously = continuously;
}

void SequentialInvoker::clear()
{
	while (!pimpl->m_action_list.empty())
		pimpl->popFront();

	pimpl->m_TargetNode->stopAllActions();
	pimpl->eraseCurrent();
}

const std::string & SequentialInvoker::getType() const
{
	return Type;
}

bool SequentialInvoker::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void SequentialInvoker::vPostInit()
{
	pimpl->init();
}

const std::string SequentialInvoker::Type = "SequentialInvokerComponent";
