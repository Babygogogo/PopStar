#ifndef __STAR_MATRIX__
#define __STAR_MATRIX__

#include <memory>

#include "Script.h"
#include "../Common/IUpdateable.h"

class StarMatrix final: public Script, public IUpdateable
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

private:
	StarMatrix(GameObject *game_object);

	virtual void update(const std::time_t& time_ms);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR_MATRIX__
