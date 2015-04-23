#include "Stepper.h"
#include "cocos2d.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"

#include <list>

struct Stepper::impl
{
public:
	impl(){};
	~impl();

	void onPreviousStepFinished();

	void pushBackStep(cocos2d::Sequence *step);
	void popFrontStep();

	bool m_is_running_previous_step{ false };

	std::list<cocos2d::Sequence*> m_step_list;
};

void Stepper::impl::onPreviousStepFinished()
{
	m_is_running_previous_step = false;
	popFrontStep();
}

void Stepper::impl::popFrontStep()
{
	if (m_step_list.empty())
		return;

	m_step_list.front()->release();
	m_step_list.pop_front();
}

void Stepper::impl::pushBackStep(cocos2d::Sequence *step)
{
	step->retain();
	m_step_list.push_back(step);
}

Stepper::impl::~impl()
{
	while (!m_step_list.empty())
		popFrontStep();
}

Stepper::~Stepper()
{

}

Stepper::Stepper(GameObject *game_object) :Component("Stepper", game_object), pimpl(new impl)
{

}

bool Stepper::nextStep()
{
	if (pimpl->m_is_running_previous_step || pimpl->m_step_list.empty())
		return false;

	if (auto node = m_game_object->getComponent<DisplayNode>()->getAs<cocos2d::Node>()){
		pimpl->m_is_running_previous_step = true;
		node->runAction(pimpl->m_step_list.front());

		return true;
	}

	return false;
}

void Stepper::addStep(float duration, float x, float y, std::function<void()> &&callback /*= nullptr*/)
{
	auto move_to = cocos2d::MoveTo::create(duration, { x, y });
	auto step_finish_callback = cocos2d::CallFunc::create([this]{pimpl->onPreviousStepFinished(); });

	auto step = callback ?
		cocos2d::Sequence::create(move_to, cocos2d::CallFunc::create(callback), step_finish_callback, nullptr) :
		cocos2d::Sequence::create(move_to,										step_finish_callback, nullptr);

	pimpl->pushBackStep(step);
}
