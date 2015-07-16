#ifndef __BASE_SCRIPT_COMPONENT__
#define __BASE_SCRIPT_COMPONENT__

#include "ActorComponent.h"

class BaseScriptComponent : public ActorComponent
{
public:
	virtual ~BaseScriptComponent();

protected:
	BaseScriptComponent();
	BaseScriptComponent(std::string &&name, Actor *game_object) :ActorComponent(std::move(name), game_object){};

	virtual bool vInit(tinyxml2::XMLElement *xmlElement);

	//This function should be removed in the future.
	virtual const std::string & getType() const override;
};

#endif // !__BASE_SCRIPT_COMPONENT__
