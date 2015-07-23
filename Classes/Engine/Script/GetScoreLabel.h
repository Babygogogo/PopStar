#ifndef __GET_SCORE_LABEL__
#define __GET_SCORE_LABEL__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class GetScoreLabel final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	GetScoreLabel(const GetScoreLabel&) = delete;
	GetScoreLabel(GetScoreLabel&&) = delete;
	GetScoreLabel& operator=(const GetScoreLabel&) = delete;
	GetScoreLabel& operator=(GetScoreLabel&&) = delete;

public:
	GetScoreLabel(Actor *game_object);
	~GetScoreLabel();

	virtual const std::string & getType() const override;

	static const std::string Type;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__GET_SCORE_LABEL__
