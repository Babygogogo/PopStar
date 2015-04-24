#include "SequentialInvoker.h"
#include "cocos2d.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"

#include <list>

struct SequentialInvoker::impl
{
	impl(GameObject *game_object, cocos2d::Node *target_node) :m_target(game_object), m_target_node(target_node){};
	~impl();

	void pushBackStep(cocos2d::Sequence *step);
	void popFrontStep();

	GameObject *m_target;
	cocos2d::Node *m_target_node;
	bool m_is_front_step_invoked{ false };
	std::list<cocos2d::Sequence*> m_step_list;
};

void SequentialInvoker::impl::popFrontStep()
{
	if (m_step_list.empty())
		return;

	m_is_front_step_invoked = false;
	m_step_list.front()->release();
	m_step_list.pop_front();
}

void SequentialInvoker::impl::pushBackStep(cocos2d::Sequence *step)
{
	step->retain();
	m_step_list.push_back(step);
}

SequentialInvoker::impl::~impl()
{
	while (!m_step_list.empty())
		popFrontStep();
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
	if (pimpl->m_step_list.empty())
		return false;

	return pimpl->m_is_front_step_invoked && !pimpl->m_step_list.front()->isDone();
}

bool SequentialInvoker::invoke()
{
	if (isInvoking())
		return false;

	if (pimpl->m_is_front_step_invoked)
		pimpl->popFrontStep();

	if (!pimpl->m_step_list.empty()){
		pimpl->m_target_node->runAction(pimpl->m_step_list.front());
		return pimpl->m_is_front_step_invoked = true;
	}

	return false;
}

void SequentialInvoker::addMoveTo(float duration, float x, float y, std::function<void()> &&callback /*= nullptr*/)
{
	auto move_to = cocos2d::MoveTo::create(duration, { x, y });
	auto step = callback ?
		cocos2d::Sequence::create(move_to, cocos2d::CallFunc::create(callback),nullptr) :
		cocos2d::Sequence::create(move_to, nullptr);

	pimpl->pushBackStep(step);
}
