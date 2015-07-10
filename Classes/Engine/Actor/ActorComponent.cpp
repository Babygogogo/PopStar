#include <cassert>

#include "ActorComponent.h"

ActorComponent::ActorComponent(std::string &&name, Actor *game_object) : m_game_object(game_object)
{

}

ActorComponent::~ActorComponent()
{

}

void ActorComponent::setOwner(std::weak_ptr<Actor> && actor)
{
	assert(!actor.expired());

	m_Actor = std::move(actor);
}

void ActorComponent::vPostInit()
{

}

void ActorComponent::vOnChanged()
{

}
