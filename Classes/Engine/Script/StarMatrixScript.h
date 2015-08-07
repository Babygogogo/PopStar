#ifndef __STAR_MATRIX_SCRIPT__
#define __STAR_MATRIX_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class StarMatrixScript final : public BaseScriptComponent
{
public:
	StarMatrixScript();
	~StarMatrixScript();

	static const std::string Type;

	void reset();

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	StarMatrixScript(const StarMatrixScript&) = delete;
	StarMatrixScript(StarMatrixScript&&) = delete;
	StarMatrixScript& operator=(const StarMatrixScript&) = delete;
	StarMatrixScript& operator=(StarMatrixScript&&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct StarMatrixScriptImpl;
	std::unique_ptr<StarMatrixScriptImpl> pimpl;
};

#endif // !__STAR_MATRIX_SCRIPT__
