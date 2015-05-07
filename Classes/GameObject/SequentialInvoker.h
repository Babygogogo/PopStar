#ifndef __SEQUENTIAL_INVOKER__
#define __SEQUENTIAL_INVOKER__

#include <memory>
#include <functional>

#include "Component.h"
#include "cocos2d.h"

class SequentialInvoker final :public Component
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	SequentialInvoker(const SequentialInvoker&) = delete;
	SequentialInvoker(SequentialInvoker&&) = delete;
	SequentialInvoker& operator=(const SequentialInvoker&) = delete;
	SequentialInvoker& operator=(SequentialInvoker&&) = delete;
	
public:
	~SequentialInvoker();
		
	void addMoveTo(float duration_s, float x, float y, std::function<void()> &&callback = nullptr);
	void addDelay(float delay_s);
	void addCallback(std::function<void()> &&callback);
	void addFiniteTimeAction(cocos2d::FiniteTimeAction* action);
	void clear();

	void setInvokeContinuously(bool continuously);
	bool invoke();
	bool isInvoking() const;

private:
	SequentialInvoker(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__SEQUENTIAL_INVOKER__
