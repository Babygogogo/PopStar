#include "BaseScriptComponent.h"

const std::string & BaseScriptComponent::Type = "BaseScriptComponent";

BaseScriptComponent::~BaseScriptComponent()
{

}

const std::string & BaseScriptComponent::getType() const
{
	return Type;
}

bool BaseScriptComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}
