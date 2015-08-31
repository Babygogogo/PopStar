#ifndef __GET_SCORE_LABEL_SCRIPT__
#define __GET_SCORE_LABEL_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class GetScoreLabelScript final :public BaseScriptComponent
{
public:
	GetScoreLabelScript();
	~GetScoreLabelScript();

	//The type name of this component. Same as the class name.
	static const std::string Type;

	//Disable copy/move constructor and operator=.
	GetScoreLabelScript(const GetScoreLabelScript&) = delete;
	GetScoreLabelScript(GetScoreLabelScript&&) = delete;
	GetScoreLabelScript& operator=(const GetScoreLabelScript&) = delete;
	GetScoreLabelScript& operator=(GetScoreLabelScript&&) = delete;

private:
	//Override functions.
	virtual const std::string & getType() const override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct GetScoreLabelScriptImpl;
	std::shared_ptr<GetScoreLabelScriptImpl> pimpl;
};

#endif // !__GET_SCORE_LABEL_SCRIPT__
