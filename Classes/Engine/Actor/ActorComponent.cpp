#include <cassert>

#include "ActorComponent.h"

ActorComponent::ActorComponent(std::string &&name, Actor *game_object) : m_game_object(game_object)
{

}

ActorComponent::ActorComponent()
{

}

ActorComponent::~ActorComponent()
{

}

void ActorComponent::setOwner(std::weak_ptr<Actor> && owner)
{
	assert(!owner.expired());

	m_Actor = std::move(owner);
}

void ActorComponent::vPostInit()
{

}

void ActorComponent::vOnChanged()
{

}
