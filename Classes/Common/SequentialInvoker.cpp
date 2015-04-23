#include "SequentialInvoker.h"
#include "cocos2d.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"

#include <list>

struct SequentialInvoker::impl
{
	impl(GameObject *game_object) :m_target(game_object){};
	~impl();

	void onPreviousStepFinished();

	void pushBackStep(cocos2d::Sequence *step);
	void popFrontStep();

	GameObject *m_target;
	bool m_is_running_previous_step{ false };
	std::list<cocos2d::Sequence*> m_step_list;
};

void SequentialInvoker::impl::onPreviousStepFinished()
{
	m_is_running_previous_step = false;
	popFrontStep();
}

void SequentialInvoker::impl::popFrontStep()
{
	if (m_step_list.empty())
		return;

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

SequentialInvoker::SequentialInvoker(GameObject *game_object) :Object("SequentialInvoker"), pimpl(new impl(game_object))
{

}

SequentialInvoker::~SequentialInvoker()
{

}

bool SequentialInvoker::isInvoking() const
{
	return pimpl->m_is_running_previous_step;
}

bool SequentialInvoker::invoke()
{
	if (pimpl->m_is_running_previous_step || pimpl->m_step_list.empty())
		return false;

	if (auto node = pimpl->m_target->getComponent<DisplayNode>()->getAs<cocos2d::Node>()){
		pimpl->m_is_running_previous_step = true;
		node->runAction(pimpl->m_step_list.front());

		return true;
	}

	return false;
}

void SequentialInvoker::addMoveTo(float duration, float x, float y, std::function<void()> &&callback /*= nullptr*/)
{
	auto move_to = cocos2d::MoveTo::create(duration, { x, y });
	auto step_finish_callback = cocos2d::CallFunc::create([this]{pimpl->onPreviousStepFinished(); });

	auto step = callback ?
		cocos2d::Sequence::create(move_to, cocos2d::CallFunc::create(callback), step_finish_callback, nullptr) :
		cocos2d::Sequence::create(move_to, step_finish_callback, nullptr);

	pimpl->pushBackStep(step);
}

std::unique_ptr<SequentialInvoker> SequentialInvoker::create(GameObject *game_object)
{
	return std::unique_ptr<SequentialInvoker>(new SequentialInvoker(game_object));
}
