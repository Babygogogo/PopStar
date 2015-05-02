#ifndef __STATUS_LAYER__
#define __STATUS_LAYER__

#include "Script.h"

#include <memory>

class StatusLayer final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StatusLayer(const StatusLayer&) = delete;
	StatusLayer(StatusLayer&&) = delete;
	StatusLayer& operator=(const StatusLayer&) = delete;
	StatusLayer& operator=(StatusLayer&&) = delete;
	
public:
	~StatusLayer();

private:
	StatusLayer(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STATUS_LAYER__
