#ifndef __PUZZLE_SCENE__
#define __PUZZLE_SCENE__

#include "Script.h"

#include <memory>

class PuzzleScene final :public Script
{
	friend class GameObject;

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
	PuzzleScene(GameObject* game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__PUZZLE_SCENE__
