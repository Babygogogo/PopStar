#ifndef __PUZZLE_MATRIX_LAYER__
#define __PUZZLE_MATRIX_LAYER__

#include <memory>
#include "../Actor/BaseScriptComponent.h"

class MatrixLayer final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	MatrixLayer(const MatrixLayer&) = delete;
	MatrixLayer(MatrixLayer&&) = delete;
	MatrixLayer& operator=(const MatrixLayer&) = delete;
	MatrixLayer& operator=(MatrixLayer&&) = delete;
	
public:
	~MatrixLayer();

private:
	MatrixLayer(Actor *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__PUZZLE_MATRIX_LAYER__
