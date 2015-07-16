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

const std::string & BaseScriptComponent::getType() const
{
	static std::string nullString{};
	return nullString;
}
