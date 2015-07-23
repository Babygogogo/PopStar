#ifndef __GAME_OVER_LABEL_SCRIPT__
#define __GAME_OVER_LABEL_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class GameOverLabelScript final :public BaseScriptComponent
{
public:
	GameOverLabelScript();
	~GameOverLabelScript();

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	GameOverLabelScript(const GameOverLabelScript&) = delete;
	GameOverLabelScript(GameOverLabelScript&&) = delete;
	GameOverLabelScript& operator=(const GameOverLabelScript&) = delete;
	GameOverLabelScript& operator=(GameOverLabelScript&&) = delete;

private:
	//Override functions.
	virtual const std::string & getType() const override;
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct GameOverLabelScriptImpl;
	std::unique_ptr<GameOverLabelScriptImpl> pimpl;
};

#endif // !__GAME_OVER_LABEL_SCRIPT__
