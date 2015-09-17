namespace tinyxml2{
	class XMLElement;
}

class RelativePosition
{
public:
	RelativePosition();
	RelativePosition(tinyxml2::XMLElement *	positionElement);
	~RelativePosition();

	void initialize(tinyxml2::XMLElement * positionElement);

	float m_NormalizedScreenOffsetX{ 0.0f }, m_NormalizedScreenOffsetY{ 0.0f };
	float m_NormalizedNodeOffsetX{ 0.0f }, m_NormalizedNodeOffsetY{ 0.0f };
	float m_PixelOffsetX{ 0.0f }, m_PixelOffsetY{ 0.0f };
};
