#include "BaseScriptComponent.h"

BaseScriptComponent::BaseScriptComponent() : ActorComponent()
{
}

BaseScriptComponent::~BaseScriptComponent()
{
}

bool BaseScriptComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void BaseScriptComponent::vPostInit()
{
}
