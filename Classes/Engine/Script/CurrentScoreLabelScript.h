#ifndef __CURRENT_SCORE_LABEL_SCRIPT__
#define __CURRENT_SCORE_LABEL_SCRIPT__

#include <memory>
#include <string>

#include "../Actor/BaseScriptComponent.h"

class CurrentScoreLabelScript final : public BaseScriptComponent
{
public:
	CurrentScoreLabelScript();
	~CurrentScoreLabelScript();

	static const std::string Type;

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	CurrentScoreLabelScript(const CurrentScoreLabelScript &) = delete;
	CurrentScoreLabelScript(CurrentScoreLabelScript &&) = delete;
	CurrentScoreLabelScript & operator=(const CurrentScoreLabelScript &) = delete;
	CurrentScoreLabelScript & operator=(CurrentScoreLabelScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct CurrentScoreLabelScriptImpl;
	std::shared_ptr<CurrentScoreLabelScriptImpl> pimpl;
};

#endif // !__CURRENT_SCORE_LABEL_SCRIPT__
