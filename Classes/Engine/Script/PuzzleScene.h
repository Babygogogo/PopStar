#ifndef __PUZZLE_SCENE__
#define __PUZZLE_SCENE__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class PuzzleScene final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	PuzzleScene(const PuzzleScene&) = delete;
	PuzzleScene(PuzzleScene&&) = delete;
	PuzzleScene& operator=(const PuzzleScene&) = delete;
	PuzzleScene& operator=(PuzzleScene&&) = delete;

public:
	~PuzzleScene();

private:
	PuzzleScene(Actor* game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__PUZZLE_SCENE__
