#ifndef __BUTTON_START_GAME_SCRIPT__
#define __BUTTON_START_GAME_SCRIPT__

#include <memory>
#include <string>

#include "../Actor/BaseScriptComponent.h"

class ButtonStartGameScript final : public BaseScriptComponent
{
public:
	ButtonStartGameScript();
	~ButtonStartGameScript();

	virtual const std::string & getType() const override;

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	ButtonStartGameScript(const ButtonStartGameScript&) = delete;
	ButtonStartGameScript(ButtonStartGameScript&&) = delete;
	ButtonStartGameScript& operator=(const ButtonStartGameScript&) = delete;
	ButtonStartGameScript& operator=(ButtonStartGameScript&&) = delete;

private:
	//Override functions.
	virtual void vPostInit() override;

	//Implementation stuff.
	struct ButtonStartGameScriptImpl;
	std::unique_ptr<ButtonStartGameScriptImpl> pimpl;
};

#endif // !__BUTTON_START_GAME_SCRIPT__
