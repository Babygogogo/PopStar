#ifndef __SEQUENTIAL_INVOKER__
#define __SEQUENTIAL_INVOKER__

#include <memory>
#include <functional>

#include "Object.h"

class GameObject;

class SequentialInvoker final : public Object
{
	friend class SequentialInvokerContainer;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	SequentialInvoker(const SequentialInvoker&) = delete;
	SequentialInvoker(SequentialInvoker&&) = delete;
	SequentialInvoker& operator=(const SequentialInvoker&) = delete;
	SequentialInvoker& operator=(SequentialInvoker&&) = delete;
	
public:
	~SequentialInvoker();

	bool isInvoking() const;
	void addMoveTo(float duration, float x, float y, std::function<void()> &&callback = nullptr);

	bool invoke();

private:
	static std::unique_ptr<SequentialInvoker> create(GameObject *game_object);
	SequentialInvoker(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__SEQUENTIAL_INVOKER__
