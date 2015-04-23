#ifndef __SEQUENTIAL_INVOKER_CONTAINER__
#define __SEQUENTIAL_INVOKER_CONTAINER__

#include <memory>

#include "Object.h"

class GameObject;
class SequentialInvoker;

class SequentialInvokerContainer final : public Object
{
	friend class SingletonContainer;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	SequentialInvokerContainer(const SequentialInvokerContainer&) = delete;
	SequentialInvokerContainer(SequentialInvokerContainer&&) = delete;
	SequentialInvokerContainer& operator=(const SequentialInvokerContainer&) = delete;
	SequentialInvokerContainer& operator=(SequentialInvokerContainer&&) = delete;
	
public:
	~SequentialInvokerContainer();

	SequentialInvoker *registerTarget(GameObject *game_object);
	SequentialInvoker *getInvokerBy(GameObject *game_object);
	void deleteTarget(GameObject *game_object);

private:
	static std::unique_ptr<SequentialInvokerContainer> create();
	SequentialInvokerContainer();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__SEQUENTIAL_INVOKER_CONTAINER__
