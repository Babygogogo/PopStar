#ifndef __STATUS_LAYER__
#define __STATUS_LAYER__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class StatusLayer final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StatusLayer(const StatusLayer&) = delete;
	StatusLayer(StatusLayer&&) = delete;
	StatusLayer& operator=(const StatusLayer&) = delete;
	StatusLayer& operator=(StatusLayer&&) = delete;

public:
	StatusLayer(Actor *game_object);
	~StatusLayer();

	virtual const std::string & getType() const override;

	static const std::string Type;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STATUS_LAYER__
