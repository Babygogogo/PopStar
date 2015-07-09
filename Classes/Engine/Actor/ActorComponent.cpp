#include "ActorComponent.h"

ActorComponent::ActorComponent(std::string &&name, Actor *game_object) :Object(std::move(name)), m_game_object(game_object)
{
}

ActorComponent::~ActorComponent()
{
}
