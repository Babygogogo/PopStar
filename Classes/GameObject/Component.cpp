#include "Component.h"

Component::Component(const std::string& name, GameObject *game_object) :Object(name), m_game_object(game_object)
{
}

Component::~Component()
{
}
