#ifndef __TITLE_SCENE__
#define __TITLE_SCENE__

#include <memory>
#include "Script.h"

class TitleScene final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	TitleScene(const TitleScene&) = delete;
	TitleScene(TitleScene&&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;
	TitleScene& operator=(TitleScene&&) = delete;
	
public:
	~TitleScene();

protected:
	virtual void update(const std::time_t& time_ms) override{ return; };

private:
	TitleScene(GameObject* game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__TITLE_SCENE__
