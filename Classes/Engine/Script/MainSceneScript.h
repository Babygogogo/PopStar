#ifndef __MAIN_SCENE__
#define __MAIN_SCENE__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class MainSceneScript final :public BaseScriptComponent
{
public:
	MainSceneScript();
	~MainSceneScript();

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	MainSceneScript(const MainSceneScript&) = delete;
	MainSceneScript(MainSceneScript&&) = delete;
	MainSceneScript& operator=(const MainSceneScript&) = delete;
	MainSceneScript& operator=(MainSceneScript&&) = delete;

private:
	//Override functions.
	virtual const std::string & getType() const override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct MainSceneScriptImpl;
	std::unique_ptr<MainSceneScriptImpl> pimpl;
};

#endif // !__MAIN_SCENE__
