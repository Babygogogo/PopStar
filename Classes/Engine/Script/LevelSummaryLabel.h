#ifndef __LEVEL_SUMMARY_LABEL__
#define __LEVEL_SUMMARY_LABEL__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class LevelSummaryLabel final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	LevelSummaryLabel(const LevelSummaryLabel&) = delete;
	LevelSummaryLabel(LevelSummaryLabel&&) = delete;
	LevelSummaryLabel& operator=(const LevelSummaryLabel&) = delete;
	LevelSummaryLabel& operator=(LevelSummaryLabel&&) = delete;

public:
	LevelSummaryLabel(Actor *game_object);
	~LevelSummaryLabel();

	virtual const std::string & getType() const override;

	static const std::string Type;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__LEVEL_SUMMARY_LABEL__
