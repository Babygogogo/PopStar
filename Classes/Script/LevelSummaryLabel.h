#ifndef __LEVEL_SUMMARY_LABEL__
#define __LEVEL_SUMMARY_LABEL__

#include "Script.h"

#include <memory>

class LevelSummaryLabel final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	LevelSummaryLabel(const LevelSummaryLabel&) = delete;
	LevelSummaryLabel(LevelSummaryLabel&&) = delete;
	LevelSummaryLabel& operator=(const LevelSummaryLabel&) = delete;
	LevelSummaryLabel& operator=(LevelSummaryLabel&&) = delete;
	
public:
	~LevelSummaryLabel();

private:
	LevelSummaryLabel(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__LEVEL_SUMMARY_LABEL__
