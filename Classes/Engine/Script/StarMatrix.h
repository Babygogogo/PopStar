#ifndef __STAR_MATRIX__
#define __STAR_MATRIX__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class StarMatrix final : public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StarMatrix(const StarMatrix&) = delete;
	StarMatrix(StarMatrix&&) = delete;
	StarMatrix& operator=(const StarMatrix&) = delete;
	StarMatrix& operator=(StarMatrix&&) = delete;

public:
	StarMatrix(Actor *game_object);
	~StarMatrix();

	static const std::string Type;

	void reset();

	virtual const std::string & getType() const override;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR_MATRIX__
