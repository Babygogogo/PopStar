#ifndef __TITLE_SCENE_SCRIPT__
#define __TITLE_SCENE_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class TitleSceneScript final : public BaseScriptComponent
{
public:
	TitleSceneScript();
	~TitleSceneScript();

	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	TitleSceneScript(const TitleSceneScript&) = delete;
	TitleSceneScript(TitleSceneScript&&) = delete;
	TitleSceneScript& operator=(const TitleSceneScript&) = delete;
	TitleSceneScript& operator=(TitleSceneScript&&) = delete;

private:
	//Override functions.
	virtual void vPostInit() override;

	//Implementation stuff.
	struct TitleSceneScriptImpl;
	std::unique_ptr<TitleSceneScriptImpl> pimpl;
};

#endif // !__TITLE_SCENE_SCRIPT__
