#ifndef __LEVEL_SUMMARY_LABEL_SCRIPT__
#define __LEVEL_SUMMARY_LABEL_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class LevelSummaryLabelScript final :public BaseScriptComponent
{
public:
	LevelSummaryLabelScript();
	~LevelSummaryLabelScript();

	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	LevelSummaryLabelScript(const LevelSummaryLabelScript&) = delete;
	LevelSummaryLabelScript(LevelSummaryLabelScript&&) = delete;
	LevelSummaryLabelScript& operator=(const LevelSummaryLabelScript&) = delete;
	LevelSummaryLabelScript& operator=(LevelSummaryLabelScript&&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct LevelSummaryLabelScriptImpl;
	std::shared_ptr<LevelSummaryLabelScriptImpl> pimpl;
};

#endif // !__LEVEL_SUMMARY_LABEL_SCRIPT__
