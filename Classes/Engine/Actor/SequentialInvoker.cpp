#include <cassert>
#include <list>

#include "cocos2d.h"

#include "SequentialInvoker.h"
#include "Actor.h"
#include "BaseRenderComponent.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SequentialInvokerImpl.
//////////////////////////////////////////////////////////////////////////
struct SequentialInvoker::SequentialInvokerImpl
{
	SequentialInvokerImpl();
	~SequentialInvokerImpl();

	void pushBack(cocos2d::Action *action);
	void popFront();
	void eraseCurrentAction();

	bool invoke(bool isCalledByLastAction);

	bool isInvoking() const;

	cocos2d::Node *m_TargetNode{ nullptr };
	cocos2d::Action *m_CurrentAction{ nullptr };
	bool m_IsInvokeContinuously{ false };
	std::list<cocos2d::Action*> m_ActionList;
};

SequentialInvoker::SequentialInvokerImpl::SequentialInvokerImpl()
{
}

SequentialInvoker::SequentialInvokerImpl::~SequentialInvokerImpl()
{
	while (!m_ActionList.empty())
		popFront();

	eraseCurrentAction();
}

void SequentialInvoker::SequentialInvokerImpl::popFront()
{
	if (m_ActionList.empty())
		return;

	m_ActionList.front()->release();
	m_ActionList.pop_front();
}

void SequentialInvoker::SequentialInvokerImpl::pushBack(cocos2d::Action *action)
{
	action->retain();
	m_ActionList.emplace_back(action);
}

void SequentialInvoker::SequentialInvokerImpl::eraseCurrentAction()
{
	if (!m_CurrentAction)
		return;

	m_CurrentAction->release();
	m_CurrentAction = nullptr;
}

bool SequentialInvoker::SequentialInvokerImpl::invoke(bool isCalledByLastAction)
{
	if (isCalledByLastAction && !m_IsInvokeContinuously)
		return false;
	if (isInvoking() || m_ActionList.empty())
		return false;

	m_CurrentAction = m_ActionList.front();
	m_ActionList.pop_front();
	m_TargetNode->runAction(m_CurrentAction);

	return true;
}

bool SequentialInvoker::SequentialInvokerImpl::isInvoking() const
{
	if (!m_CurrentAction)
		return false;

	return !m_CurrentAction->isDone();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of SequentialInvoker.
//////////////////////////////////////////////////////////////////////////
SequentialInvoker::SequentialInvoker() : pimpl{ std::make_shared<SequentialInvokerImpl>() }
{
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

	if (callback)
		addFiniteTimeAction(cocos2d::Sequence::create(move_to, cocos2d::CallFunc::create(callback), nullptr));
	else
		addFiniteTimeAction(move_to);
}

void SequentialInvoker::addDelay(float delay_s)
{
	assert(delay_s > 0.0f && "SequentialInvoker::addDelay() with time <= 0.");

	addFiniteTimeAction(cocos2d::DelayTime::create(delay_s));
}

void SequentialInvoker::addCallback(std::function<void()> &&callback)
{
	addFiniteTimeAction(cocos2d::CallFunc::create(callback));
}

void SequentialInvoker::addFiniteTimeAction(cocos2d::FiniteTimeAction* action)
{
	auto weakPimpl = std::weak_ptr<SequentialInvokerImpl>(pimpl);
	auto recallSelf = cocos2d::CallFunc::create([weakPimpl](){
		if (!weakPimpl.expired()){
			auto strongPimpl = weakPimpl.lock();
			strongPimpl->eraseCurrentAction();
			strongPimpl->invoke(true);
		}
	});

	pimpl->pushBack(cocos2d::Sequence::create(action, recallSelf, nullptr));

	if (pimpl->m_IsInvokeContinuously)
		invoke();
}

void SequentialInvoker::setInvokeContinuously(bool continuously)
{
	pimpl->m_IsInvokeContinuously = continuously;
}

void SequentialInvoker::clearAllActions()
{
	while (!pimpl->m_ActionList.empty())
		pimpl->popFront();

	pimpl->m_TargetNode->stopAllActions();
	pimpl->eraseCurrentAction();
}

bool SequentialInvoker::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void SequentialInvoker::vPostInit()
{
	pimpl->m_TargetNode = m_Actor.lock()->getRenderComponent()->getSceneNode();
}

const std::string & SequentialInvoker::getType() const
{
	return Type;
}

const std::string SequentialInvoker::Type = "SequentialInvokerComponent";
