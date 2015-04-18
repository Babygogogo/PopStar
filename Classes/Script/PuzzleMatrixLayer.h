#ifndef __PUZZLE_MATRIX_LAYER__
#define __PUZZLE_MATRIX_LAYER__

#include <memory>
#include "Script.h"

class PuzzleMatrixLayer final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	PuzzleMatrixLayer(const PuzzleMatrixLayer&) = delete;
	PuzzleMatrixLayer(PuzzleMatrixLayer&&) = delete;
	PuzzleMatrixLayer& operator=(const PuzzleMatrixLayer&) = delete;
	PuzzleMatrixLayer& operator=(PuzzleMatrixLayer&&) = delete;
	
public:
	~PuzzleMatrixLayer();

protected:
	virtual void update(const std::time_t& time_ms) override;

private:
	PuzzleMatrixLayer(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__PUZZLE_MATRIX_LAYER__
