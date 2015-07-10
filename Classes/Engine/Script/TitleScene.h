#ifndef __TITLE_SCENE__
#define __TITLE_SCENE__

#include <memory>
#include "../Actor/BaseScriptComponent.h"

class TitleScene final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	
public:
	~TitleScene();

private:
	TitleScene(Actor* game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__TITLE_SCENE__
