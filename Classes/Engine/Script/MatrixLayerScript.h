#ifndef __MATRIX_LAYER_SCRIPT__
#define __MATRIX_LAYER_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class MatrixLayerScript final :public BaseScriptComponent
{
public:
	MatrixLayerScript();
	~MatrixLayerScript();

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	MatrixLayerScript(const MatrixLayerScript&) = delete;
	MatrixLayerScript(MatrixLayerScript&&) = delete;
	MatrixLayerScript& operator=(const MatrixLayerScript&) = delete;
	MatrixLayerScript& operator=(MatrixLayerScript&&) = delete;

private:
	//Override functions.
	virtual const std::string & getType() const override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MatrixLayerScriptImpl;
	std::unique_ptr<MatrixLayerScriptImpl> pimpl;
};

#endif // !__MATRIX_LAYER_SCRIPT__
