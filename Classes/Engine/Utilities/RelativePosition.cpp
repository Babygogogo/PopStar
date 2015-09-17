#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "RelativePosition.h"

RelativePosition::RelativePosition()
{
}

RelativePosition::RelativePosition(tinyxml2::XMLElement * positionElement)
{
	initialize(positionElement);
}

RelativePosition::~RelativePosition()
{
}

void RelativePosition::initialize(tinyxml2::XMLElement * positionElement)
{
	m_NormalizedScreenOffsetX = positionElement->FloatAttribute("NormalizedScreenOffsetX");
	m_NormalizedScreenOffsetY = positionElement->FloatAttribute("NormalizedScreenOffsetY");

	m_NormalizedNodeOffsetX = positionElement->FloatAttribute("NormalizedNodeOffsetX");
	m_NormalizedNodeOffsetY = positionElement->FloatAttribute("NormalizedNodeOffsetY");

	m_PixelOffsetX = positionElement->FloatAttribute("PixelOffsetX");
	m_PixelOffsetY = positionElement->FloatAttribute("PixelOffsetY");
}
