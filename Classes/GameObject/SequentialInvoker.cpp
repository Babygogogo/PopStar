#include "SequentialInvoker.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Common/SingletonContainer.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/Event.h"

#include <list>

struct SequentialInvoker::impl
{
	impl(GameObject *game_object, cocos2d::Node *target_node);
	~impl();

	void pushBackStep(cocos2d::Action *step);
	void popFrontStep();

	bool invoke(bool is_called_by_event_dispatcher);
	bool isInvoking() const;

	cocos2d::CallFunc *createDispatchCallback() const;

	GameObject *m_target;
	cocos2d::Node *m_target_node;
	bool m_invoke_continuously{ false };
	bool m_is_front_step_invoked{ false };
	std::list<cocos2d::Action*> m_step_list;
};

SequentialInvoker::impl::impl(GameObject *game_object, cocos2d::Node *target_node) :m_target(game_object), m_target_node(target_node)
{
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(
		EventType::SequentialInvokerFinishOneAction, this, [this](Event *e){invoke(true); });
}

SequentialInvoker::impl::~impl()
{
	while (!m_step_list.empty())
		popFrontStep();

	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void SequentialInvoker::impl::popFrontStep()
{
	if (m_step_list.empty())
		return;

	m_is_front_step_invoked = false;
	m_step_list.front()->release();
	m_step_list.pop_front();
}

void SequentialInvoker::impl::pushBackStep(cocos2d::Action *step)
{
	step->retain();
	m_step_list.push_back(step);
}

bool SequentialInvoker::impl::invoke(bool is_called_by_event_dispatcher)
{
	if (is_called_by_event_dispatcher && !m_invoke_continuously)
		return false;
	if (isInvoking())
		return false;

	if (m_is_front_step_invoked)
		popFrontStep();

	if (!m_step_list.empty()){
		m_target_node->runAction(m_step_list.front());
		return m_is_front_step_invoked = true;
	}

	return false;
}

bool SequentialInvoker::impl::isInvoking() const
{
	if (m_step_list.empty())
		return false;

	return m_is_front_step_invoked && !m_step_list.front()->isDone();
}

cocos2d::CallFunc * SequentialInvoker::impl::createDispatchCallback() const
{
	return cocos2d::CallFunc::create([this]{if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->dispatch(Event::create(EventType::SequentialInvokerFinishOneAction), const_cast<SequentialInvoker::impl*>(this)); });
}

SequentialInvoker::SequentialInvoker(GameObject *game_object) :Component("SequentialInvoker2", game_object)
{
	auto display_node = game_object->addComponent<DisplayNode>();
	auto target_node = display_node->getAs<cocos2d::Node>();
	if (!target_node)
		target_node = display_node->initAs<cocos2d::Node>();

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

void SequentialInvoker::addMoveTo(float duration, float x, float y, std::function<void()> &&callback /*= nullptr*/)
{
	auto move_to = cocos2d::MoveTo::create(duration, { x, y });
	auto step = callback ?
		cocos2d::Sequence::create(move_to, cocos2d::CallFunc::create(callback), nullptr) :
		cocos2d::Sequence::create(move_to, nullptr);

	addFiniteTimeAction(step);
}

void SequentialInvoker::addCallback(std::function<void()> &&callback)
{
	addFiniteTimeAction(cocos2d::CallFunc::create(callback));
}

void SequentialInvoker::addFiniteTimeAction(cocos2d::FiniteTimeAction* action)
{
	pimpl->pushBackStep(cocos2d::Sequence::create(action, pimpl->createDispatchCallback(), nullptr));
	if (pimpl->m_invoke_continuously)
		invoke();
}

void SequentialInvoker::setInvokeContinuously(bool continuously)
{
	pimpl->m_invoke_continuously = continuously;
}
