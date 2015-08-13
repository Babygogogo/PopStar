#ifndef __TARGET_SCORE_LABEL_SCRIPT__
#define __TARGET_SCORE_LABEL_SCRIPT__

#include <memory>
#include <string>

#include "../Actor/BaseScriptComponent.h"

class TargetScoreLabelScript final : public BaseScriptComponent
{
public:
	TargetScoreLabelScript();
	~TargetScoreLabelScript();

	static const std::string Type;

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	TargetScoreLabelScript(const TargetScoreLabelScript &) = delete;
	TargetScoreLabelScript(TargetScoreLabelScript &&) = delete;
	TargetScoreLabelScript & operator=(const TargetScoreLabelScript &) = delete;
	TargetScoreLabelScript & operator=(TargetScoreLabelScript &&) = delete;

private:
	//Override functions.
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TargetScoreLabelScriptImpl;
	std::unique_ptr<TargetScoreLabelScriptImpl> pimpl;
};

#endif // !__TARGET_SCORE_LABEL_SCRIPT__
