#ifndef __I_UPDATEABLE__
#define __I_UPDATEABLE__

#include <ctime>

class IUpdateable
{
protected:
	inline virtual void update(const std::time_t& time_ms) = 0;
};

#endif // !__I_UPDATEABLE__
