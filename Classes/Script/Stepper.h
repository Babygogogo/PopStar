#ifndef __STEPPER__
#define __STEPPER__

#include <memory>
#include <functional>

#include "Script.h"

class Stepper final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	Stepper(const Stepper&) = delete;
	Stepper(Stepper&&) = delete;
	Stepper& operator=(const Stepper&) = delete;
	Stepper& operator=(Stepper&&) = delete;
	
public:
	~Stepper();

	bool nextStep();
	void addStep(float duration, float x, float y, std::function<void()> &&callback = nullptr);

private:
	Stepper(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STEPPER__
