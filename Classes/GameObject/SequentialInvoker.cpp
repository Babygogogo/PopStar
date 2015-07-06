#include "SequentialInvoker.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Engine/Utilities/SingletonContainer.h"
#include "../Engine/Event/EventDispatcher.h"
#include "../Engine/Event/EventType.h"
#include "../Engine/Event/Event.h"

#include <list>

struct SequentialInvoker::impl
{
	impl(GameObject *game_object, cocos2d::Node *target_node);
	~impl();

	void pushBack(cocos2d::Action *action);
	void popFront();
	void eraseCurrent();

	bool invoke(bool is_called_by_event_dispatcher);
	bool isInvoking() const;

	cocos2d::CallFunc *createDispatchCallback() const;

	GameObject *m_target{ nullptr };
	cocos2d::Node *m_target_node{ nullptr };
	cocos2d::Action *m_current_action{ nullptr };
	bool m_invoke_continuously{ false };
	std::list<cocos2d::Action*> m_action_list;
};

SequentialInvoker::impl::impl(GameObject *game_object, cocos2d::Node *target_node) :m_target(game_object), m_target_node(target_node)
{
	SingletonContainer::getInstance()->get<EventDispatcher>()->registerListener(
		EventType::SequentialInvokerFinishOneAction, this, [this](Event *e){eraseCurrent(); invoke(true); });
}

SequentialInvoker::impl::~impl()
{
	while (!m_action_list.empty())
		popFront();
	eraseCurrent();

	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void SequentialInvoker::impl::popFront()
{
	if (m_action_list.empty())
		return;

	m_action_list.front()->release();
	m_action_list.pop_front();
}

void SequentialInvoker::impl::pushBack(cocos2d::Action *action)
{
	action->retain();
	m_action_list.push_back(action);
}

void SequentialInvoker::impl::eraseCurrent()
{
	if (!m_current_action)
		return;

	m_current_action->release();
	m_current_action = nullptr;
}

bool SequentialInvoker::impl::invoke(bool is_called_by_event_dispatcher)
{
	if (is_called_by_event_dispatcher && !m_invoke_continuously)
		return false;
	if (isInvoking() || m_action_list.empty())
		return false;

	m_current_action = m_action_list.front();
	m_action_list.pop_front();
	m_target_node->runAction(m_current_action);

	return true;
}

bool SequentialInvoker::impl::isInvoking() const
{
	if (!m_current_action)
		return false;

	return !m_current_action->isDone();
}

cocos2d::CallFunc * SequentialInvoker::impl::createDispatchCallback() const
{
	return cocos2d::CallFunc::create([this]{if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<EventDispatcher>()->dispatch(Event::create(EventType::SequentialInvokerFinishOneAction), const_cast<SequentialInvoker::impl*>(this)); });
}

SequentialInvoker::SequentialInvoker(GameObject *game_object) :Component("SequentialInvoker2", game_object)
{
	auto target_node = game_object->addComponent<DisplayNode>()->getAs<cocos2d::Node>();
	if (!target_node)
		throw("Add SequentialInvoker to a GameObject without an initialized DisplayNode.");

	pimpl.reset(new impl(game_object, target_node));
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

	pimpl->m_target_node->stopAllActions();
	pimpl->eraseCurrent();
}
