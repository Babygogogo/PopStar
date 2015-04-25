#include "Component.h"

Component::Component(std::string &&name, GameObject *game_object) :Object(std::move(name)), m_game_object(game_object)
{
}

Component::~Component()
{
}
