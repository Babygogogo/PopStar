#ifndef __SEQUENTIAL_INVOKER__
#define __SEQUENTIAL_INVOKER__

#include <memory>
#include <functional>

#include "ActorComponent.h"
#include "cocos2d.h"

class SequentialInvoker final :public ActorComponent
{
	friend class Actor;
	
public:
	~SequentialInvoker();

	static const std::string Type;
		
	void addMoveTo(float duration_s, float x, float y, std::function<void()> &&callback = nullptr);
	void addDelay(float delay_s);
	void addCallback(std::function<void()> &&callback);
	void addFiniteTimeAction(cocos2d::FiniteTimeAction* action);
	void clear();

	void setInvokeContinuously(bool continuously);
	bool invoke();
	bool isInvoking() const;

	//Disable copy/move constructor and operator=.
	SequentialInvoker(const SequentialInvoker&) = delete;
	SequentialInvoker(SequentialInvoker&&) = delete;
	SequentialInvoker& operator=(const SequentialInvoker&) = delete;
	SequentialInvoker& operator=(SequentialInvoker&&) = delete;

private:
	SequentialInvoker(Actor *game_object);

	virtual const std::string & getType() const;

	virtual bool vInit(tinyxml2::XMLElement *xmlElement);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__SEQUENTIAL_INVOKER__
