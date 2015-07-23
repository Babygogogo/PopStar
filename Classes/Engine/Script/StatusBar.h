#ifndef __STATUS_BAR__
#define __STATUS_BAR__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class StatusBar final : public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StatusBar(const StatusBar&) = delete;
	StatusBar(StatusBar&&) = delete;
	StatusBar& operator=(const StatusBar&) = delete;
	StatusBar& operator=(StatusBar&&) = delete;

public:
	StatusBar(Actor *game_object);
	~StatusBar();

	virtual const std::string & getType() const override;

	static const std::string Type;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STATUS_BAR__
