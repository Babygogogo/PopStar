#ifndef __GAME_OVER_LABEL__
#define __GAME_OVER_LABEL__

#include "Script.h"

#include <memory>

class GameOverLabel final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	GameOverLabel(const GameOverLabel&) = delete;
	GameOverLabel(GameOverLabel&&) = delete;
	GameOverLabel& operator=(const GameOverLabel&) = delete;
	GameOverLabel& operator=(GameOverLabel&&) = delete;
	
public:
	~GameOverLabel();

private:
	GameOverLabel(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__GAME_OVER_LABEL__
