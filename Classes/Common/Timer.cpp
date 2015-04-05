#include "Timer.h"
#include "cocos2d.h"

struct BWUpdateWrapper
{
	BWUpdateWrapper(std::function<void(const time_t&)>&& rhs){
		func = std::move(rhs);
	}
	BWUpdateWrapper(BWUpdateWrapper&& rhs){
		this->func = std::move(rhs.func);
		this->flag_deletion = std::move(rhs.flag_deletion);
	}

	BWUpdateWrapper(const BWUpdateWrapper&) = delete;
	BWUpdateWrapper& operator=(const BWUpdateWrapper&) = delete;
	BWUpdateWrapper& operator=(BWUpdateWrapper&& rhs) = delete;

	bool flag_deletion = false;
	std::function<void(time_t)> func;
};

class Timer::impl
{
public:
	impl();
	~impl();

	inline void update(float dt);

	inline void addNewUpdateObservers();
	inline void deleteUnusedUpdateObservers();
	inline void notifyUpdateObservers();

	inline time_t getCurrentTimeMS() const;
	inline int getIndex(const std::vector<long> &sequence, long num) const;

	bool m_is_updating{ false };
	bool is_initialized{ false };
	bool m_flag_for_deletion{ false };
	time_t initialize_time_ms{ 0 };
	time_t global_previous_time_ms{ 0 };
	time_t elapsed_time_ms{ 0 };

	std::unordered_map<void*, BWUpdateWrapper> m_update_observers;
	std::unordered_map<void*, BWUpdateWrapper> m_update_observers_to_add;
};

Timer::impl::impl()
{
}

Timer::impl::~impl()
{
}

void Timer::impl::update(float dt)
{
	m_is_updating = true;

	//Calculate the elapsed time
	global_previous_time_ms = elapsed_time_ms;
	elapsed_time_ms = getCurrentTimeMS() - initialize_time_ms;
	//CCLOG("global_time:%d", impl::global_elapsed_time_ms);

	//Deal with update observers
	addNewUpdateObservers();
	deleteUnusedUpdateObservers();
	notifyUpdateObservers();

	m_is_updating = false;
}

inline time_t Timer::impl::getCurrentTimeMS() const
{
	struct timeval tv;
	cocos2d::gettimeofday(&tv, nullptr);

	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

inline int Timer::impl::getIndex(const std::vector<long> &sequence, long num) const
{
	num %= sequence.back();
	for (decltype(sequence.size()) index = 0; index < sequence.size(); ++index)
		if (num < sequence[index])
			return index;

	return sequence.size() - 1;
}

void Timer::impl::addNewUpdateObservers()
{
	for (auto &observer_pair : m_update_observers_to_add)
		m_update_observers.emplace(std::move(observer_pair));
	m_update_observers_to_add.clear();
}

void Timer::impl::deleteUnusedUpdateObservers()
{
	for (auto iter = m_update_observers.begin(); iter != m_update_observers.end(); ){
		if (iter->second.flag_deletion)
			iter = m_update_observers.erase(iter);
		else
			++iter;
	}
}

void Timer::impl::notifyUpdateObservers()
{
	for (auto &observer_pair : m_update_observers)
		if (!observer_pair.second.flag_deletion)
			observer_pair.second.func(elapsed_time_ms);
}

//////////////////////////////////////////////////////////////////////////
Timer::Timer() :Object("Timer"), pimpl(new impl)
{
	CCLOG("BWTimer constructing.");
}

Timer::~Timer()
{
	CCLOG("BWTimer destructing.");
	stop();
}

inline time_t Timer::getElapsedTimeMS() const
{
	return (pimpl->is_initialized ? pimpl->elapsed_time_ms : 0);
}

bool Timer::init()
{
	if (pimpl->is_initialized)
		return false;

	pimpl->initialize_time_ms = pimpl->getCurrentTimeMS();
	pimpl->elapsed_time_ms = 0;

	cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(pimpl.get(), 0, false);

	return pimpl->is_initialized = true;
}

bool Timer::pause()
{
	if (!pimpl->is_initialized)
		return false;

	cocos2d::Director::getInstance()->getScheduler()->pauseTarget(pimpl.get());

	return true;
}

bool Timer::resume()
{
	if (!pimpl->is_initialized)
		return false;

	cocos2d::Director::getInstance()->getScheduler()->resumeTarget(pimpl.get());

	return true;
}

bool Timer::stop()
{
	if (!pimpl->is_initialized)
		return false;

	cocos2d::Director::getInstance()->getScheduler()->unscheduleUpdate(pimpl.get());

	return true;
}

void Timer::removeUpdateObserver(void *observer)
{
	if (!pimpl->m_is_updating)
		pimpl->m_update_observers.erase(observer);
	else{
		auto observer_iter = pimpl->m_update_observers.find(observer);
		if (observer_iter != pimpl->m_update_observers.end())
			observer_iter->second.flag_deletion = true;
	}
}

void Timer::registerUpdateObserverHelper(void* observer, std::function<void(const time_t&)>&& func)
{
	if (!pimpl->m_is_updating)
		pimpl->m_update_observers.emplace(std::move(observer), std::move(func));
	else
		pimpl->m_update_observers_to_add.emplace(std::move(observer), std::move(func));
}
