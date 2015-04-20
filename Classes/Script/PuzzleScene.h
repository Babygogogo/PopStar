#ifndef __PUZZLE_SCENE__
#define __PUZZLE_SCENE__

#include "Script.h"

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
};

#endif // !__PUZZLE_SCENE__
