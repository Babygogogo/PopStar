#ifndef __HIGH_SCORE_LABEL_SCRIPT__
#define __HIGH_SCORE_LABEL_SCRIPT__

#include <memory>
#include <string>

#include "../Actor/BaseScriptComponent.h"

class HighScoreLabelScript final : public BaseScriptComponent
{
public:
	HighScoreLabelScript();
	~HighScoreLabelScript();

	static const std::string Type;

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	HighScoreLabelScript(const HighScoreLabelScript &) = delete;
	HighScoreLabelScript(HighScoreLabelScript &&) = delete;
	HighScoreLabelScript & operator=(const HighScoreLabelScript &) = delete;
	HighScoreLabelScript & operator=(HighScoreLabelScript &&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct HighScoreLabelScriptImpl;
	std::shared_ptr<HighScoreLabelScriptImpl> pimpl;
};

#endif // !__HIGH_SCORE_LABEL_SCRIPT__
