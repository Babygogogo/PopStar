#ifndef __BW_TIMER__
#define __BW_TIMER__

#include <memory>
#include <functional>
#include "Object.h"

/*!
 * \class Timer
 *
 * \brief Timer of the application. Can register various observers.
 * \
 * \details
 *	In most cases, just use the global timer contained by SingletonContainer.
 *	
 * \author Babygogogo
 * \date 2015.3
 */
class Timer : public Object
{
public:
	static std::unique_ptr<Timer> create();
	virtual ~Timer();

	inline time_t getElapsedTimeMS() const;

	//////////////////////////////////////////////////////////////////////////
	//Stuff for controlling the timer.
	//////////////////////////////////////////////////////////////////////////
	bool init();
	bool pause();
	bool resume();
	bool stop();
	
	//////////////////////////////////////////////////////////////////////////
	//Stuff for registering observers.
	//////////////////////////////////////////////////////////////////////////
	template<typename T>	//The timer doesn't check whether the observer is alive; it's the resposiblity of client code to ensure that.
	void registerUpdateObserver(T *observer)
	{
		registerUpdateObserverHelper(observer, [observer](const time_t& time_ms){observer->update(time_ms); });
	}

	//Observers must call this method before its destruction.
	void removeUpdateObserver(void *observer);

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	Timer(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer& operator=(Timer&&) = delete;

private:
	Timer();
	void registerUpdateObserverHelper(void* observer, std::function<void(const time_t&)>&& func);

	class impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__BW_TIMER__
