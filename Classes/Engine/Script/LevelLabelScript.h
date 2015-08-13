#ifndef __LEVEL_LABEL_SCRIPT__
#define __LEVEL_LABEL_SCRIPT__

#include <memory>
#include <string>

#include "../Actor/BaseScriptComponent.h"

class LevelLabelScript final : public BaseScriptComponent
{
public:
	LevelLabelScript();
	~LevelLabelScript();

	static const std::string Type;

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	LevelLabelScript(const LevelLabelScript &) = delete;
	LevelLabelScript(LevelLabelScript &&) = delete;
	LevelLabelScript & operator=(const LevelLabelScript &) = delete;
	LevelLabelScript & operator=(LevelLabelScript &&) = delete;

private:
	//Override functions.
	virtual void vPostInit() override;

	//Implementation stuff.
	struct LevelLabelScriptImpl;
	std::unique_ptr<LevelLabelScriptImpl> pimpl;
};

#endif // !__LEVEL_LABEL_SCRIPT__
