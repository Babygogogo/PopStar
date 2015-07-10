#ifndef __COMBO_EFFECT__
#define __COMBO_EFFECT__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class ComboEffect final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	ComboEffect(const ComboEffect&) = delete;
	ComboEffect(ComboEffect&&) = delete;
	ComboEffect& operator=(const ComboEffect&) = delete;
	ComboEffect& operator=(ComboEffect&&) = delete;
	
public:
	~ComboEffect();

private:
	ComboEffect(Actor *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__COMBO_EFFECT__
