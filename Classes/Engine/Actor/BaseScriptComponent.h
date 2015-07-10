#ifndef __BASE_SCRIPT_COMPONENT__
#define __BASE_SCRIPT_COMPONENT__

#include "ActorComponent.h"

class BaseScriptComponent : public ActorComponent
{
public:
	virtual ~BaseScriptComponent();

	static const std::string & Type;

protected:
	BaseScriptComponent(std::string &&name, Actor *game_object) :ActorComponent(std::move(name), game_object){};

	virtual const std::string & getType() const;

	virtual bool vInit(tinyxml2::XMLElement *xmlElement);
};

#endif // !__BASE_SCRIPT_COMPONENT__
