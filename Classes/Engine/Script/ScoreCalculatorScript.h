#ifndef __SCORE_CALCULATOR_SCRIPT__
#define __SCORE_CALCULATOR_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class ScoreCalculatorScript final : public BaseScriptComponent
{
public:
	ScoreCalculatorScript();
	~ScoreCalculatorScript();

	virtual const std::string & getType() const override;

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	ScoreCalculatorScript(const ScoreCalculatorScript&) = delete;
	ScoreCalculatorScript(ScoreCalculatorScript&&) = delete;
	ScoreCalculatorScript& operator=(const ScoreCalculatorScript&) = delete;
	ScoreCalculatorScript& operator=(ScoreCalculatorScript&&) = delete;

private:

	//Implementation stuff.
	struct ScoreCalculatorScriptImpl;
	std::shared_ptr<ScoreCalculatorScriptImpl> pimpl;
};

#endif //!__SCORE_CALCULATOR_SCRIPT__
