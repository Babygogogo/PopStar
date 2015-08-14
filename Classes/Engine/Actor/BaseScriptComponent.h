#ifndef __BASE_SCRIPT_COMPONENT__
#define __BASE_SCRIPT_COMPONENT__

#include "ActorComponent.h"

class BaseScriptComponent : public ActorComponent
{
public:
	virtual ~BaseScriptComponent();

protected:

	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;
};

#endif // !__BASE_SCRIPT_COMPONENT__
