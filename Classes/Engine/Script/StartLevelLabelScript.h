#ifndef __START_LEVEL_LABEL_SCRIPT__
#define __START_LEVEL_LABEL_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class StartLevelLabelScript final :public BaseScriptComponent
{
public:
	StartLevelLabelScript();
	~StartLevelLabelScript();

	virtual const std::string & getType() const override;

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	StartLevelLabelScript(const StartLevelLabelScript&) = delete;
	StartLevelLabelScript(StartLevelLabelScript&&) = delete;
	StartLevelLabelScript& operator=(const StartLevelLabelScript&) = delete;
	StartLevelLabelScript& operator=(StartLevelLabelScript&&) = delete;

private:
	//Override functions.
	virtual void vPostInit() override;

	//Implementation stuff.
	struct StartLevelLabelScriptImpl;
	std::unique_ptr<StartLevelLabelScriptImpl> pimpl;
};

#endif // !__START_LEVEL_LABEL_SCRIPT__
