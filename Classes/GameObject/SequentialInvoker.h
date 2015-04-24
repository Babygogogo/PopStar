#ifndef __STEPPER__
#define __STEPPER__

#include <memory>
#include <functional>

#include "Component.h"

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
		
	void addMoveTo(float duration, float x, float y, std::function<void()> &&callback = nullptr);
	
	bool invoke();
	bool isInvoking() const;

private:
	SequentialInvoker(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STEPPER__
