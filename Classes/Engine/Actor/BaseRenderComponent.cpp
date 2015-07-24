#include "BaseRenderComponent.h"

const std::string BaseRenderComponent::Type = "BaseRenderComponent";

BaseRenderComponent::BaseRenderComponent()
{
}

BaseRenderComponent::BaseRenderComponent(std::string && name, Actor *game_object) : ActorComponent(std::move(name), game_object)
{
}
