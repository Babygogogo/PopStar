#ifndef __STAR_MATRIX__
#define __STAR_MATRIX__

#include <memory>

#include "Script.h"

class StarMatrix final: public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StarMatrix(const StarMatrix&) = delete;
	StarMatrix(StarMatrix&&) = delete;
	StarMatrix& operator=(const StarMatrix&) = delete;
	StarMatrix& operator=(StarMatrix&&) = delete;
	
public:
	~StarMatrix();

	void reset();

private:
	StarMatrix(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR_MATRIX__
