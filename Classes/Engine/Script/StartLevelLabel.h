#ifndef __START_LEVEL_LABEL__
#define __START_LEVEL_LABEL__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class StartLevelLabel final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StartLevelLabel(const StartLevelLabel&) = delete;
	StartLevelLabel(StartLevelLabel&&) = delete;
	StartLevelLabel& operator=(const StartLevelLabel&) = delete;
	StartLevelLabel& operator=(StartLevelLabel&&) = delete;
	
public:
	~StartLevelLabel();

private:
	StartLevelLabel(Actor *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__START_LEVEL_LABEL__
