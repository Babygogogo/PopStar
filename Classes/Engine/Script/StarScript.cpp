#include <random>
#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "StarScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/SequentialInvoker.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StarScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StarScript::StarScriptImpl
{
	StarScriptImpl();
	~StarScriptImpl();

	void setRandomColor();
	std::string getImageFileName();
	void setRandomPosition(float pos_x, float pos_y);

	float calculateMoveTime(float to_pos_x, float to_pos_y, float speed);
	void moveTo(float pos_x, float pos_y, float speed = StarScript::StarScriptImpl::s_NormalSpeedPPS);

	cocos2d::Sprite *m_sprite{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };

	bool m_is_selected{ false };
	int m_row_num{ 0 }, m_col_num{ 0 };
	float m_pos_x{ 0.0f }, m_pos_y{ 0.0f };

	int m_ColorIndex{ 0 };

	static bool s_IsStaticInitialized;
	static float s_InitialSpeedPPS;
	static float s_NormalSpeedPPS;

	struct ColorStruct{
		std::string typeName;
		std::string textureName;
		cocos2d::Color4F color4F;
	};
	static std::vector<ColorStruct> s_Colors;

	static std::random_device rd;
	static std::default_random_engine s_RandomEngine;
	static std::uniform_int_distribution<> s_RandomColorNum;
	static std::uniform_int_distribution<> s_RandomPositionOffset;
};

bool StarScript::StarScriptImpl::s_IsStaticInitialized{ false };
float StarScript::StarScriptImpl::s_InitialSpeedPPS{};
float StarScript::StarScriptImpl::s_NormalSpeedPPS{};
std::vector<StarScript::StarScriptImpl::ColorStruct> StarScript::StarScriptImpl::s_Colors;

std::random_device StarScript::StarScriptImpl::rd;
std::default_random_engine StarScript::StarScriptImpl::s_RandomEngine(rd());
std::uniform_int_distribution<> StarScript::StarScriptImpl::s_RandomColorNum;
std::uniform_int_distribution<> StarScript::StarScriptImpl::s_RandomPositionOffset;

StarScript::StarScriptImpl::StarScriptImpl()
{
}

StarScript::StarScriptImpl::~StarScriptImpl()
{
}

void StarScript::StarScriptImpl::setRandomColor()
{
	m_ColorIndex = s_RandomColorNum(s_RandomEngine);
	m_sprite->setTexture(s_Colors[m_ColorIndex].textureName);
}

void StarScript::StarScriptImpl::setRandomPosition(float pos_x, float pos_y)
{
	m_pos_x = pos_x + s_RandomPositionOffset(s_RandomEngine);
	m_pos_y = pos_y + s_RandomPositionOffset(s_RandomEngine);

	m_sprite->setPosition(m_pos_x, m_pos_y);
}

float StarScript::StarScriptImpl::calculateMoveTime(float to_pos_x, float to_pos_y, float speed)
{
	auto delta_x = m_pos_x - to_pos_x;
	auto delta_y = m_pos_y - to_pos_y;

	return sqrt(delta_x * delta_x + delta_y * delta_y) / speed;
}

void StarScript::StarScriptImpl::moveTo(float pos_x, float pos_y, float speed /*= Star::impl::MOVE_SPEED*/)
{
	if (pos_x == m_pos_x && pos_y == m_pos_y)
		return;

	m_invoker->addMoveTo(calculateMoveTime(pos_x, pos_y, speed), pos_x, pos_y);
	m_pos_x = pos_x;
	m_pos_y = pos_y;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of StarScript.
//////////////////////////////////////////////////////////////////////////
StarScript::StarScript() : pimpl{ std::make_unique<StarScriptImpl>() }
{
}

StarScript::~StarScript()
{
}

void StarScript::randomize(int row_num, int col_num, float pos_x, float pos_y)
{
	pimpl->setRandomColor();
	pimpl->m_sprite->setVisible(true);

	pimpl->m_is_selected = false;
	pimpl->m_row_num = row_num;
	pimpl->m_col_num = col_num;

	pimpl->setRandomPosition(pos_x, pos_y);
	pimpl->moveTo(pos_x, pos_y, StarScript::StarScriptImpl::s_InitialSpeedPPS);
}

void StarScript::moveTo(float pos_x, float pos_y)
{
	pimpl->moveTo(pos_x, pos_y);
}

bool StarScript::isSelected() const
{
	return pimpl->m_is_selected;
}

void StarScript::setSelected(bool selected)
{
	pimpl->m_is_selected = selected;
}

int StarScript::getRowNum() const
{
	return pimpl->m_row_num;
}

void StarScript::setRowNum(int row_num)
{
	pimpl->m_row_num = row_num;
}

int StarScript::getColNum() const
{
	return pimpl->m_col_num;
}

void StarScript::setColNum(int col_num)
{
	pimpl->m_col_num = col_num;
}

bool StarScript::canGroupWith(StarScript *star) const
{
	return this != star && pimpl->m_ColorIndex == star->pimpl->m_ColorIndex;
}

cocos2d::Color4F StarScript::getColor4F() const
{
	return pimpl->s_Colors[pimpl->m_ColorIndex].color4F;
}

float StarScript::getPositionX() const
{
	return pimpl->m_sprite->getPositionX();
}

float StarScript::getPositionY() const
{
	return pimpl->m_sprite->getPositionY();
}

void StarScript::setVisible(bool visible)
{
	pimpl->m_sprite->setVisible(visible);
}

const std::string & StarScript::getType() const
{
	return Type;
}

bool StarScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Check if the static members are initialized. If not, grab data from xml and initialize them.
	if (!pimpl->s_IsStaticInitialized){
		pimpl->s_IsStaticInitialized = true;

		//Grab the data of speed.
		auto speedElement = xmlElement->FirstChildElement("MoveSpeed");
		pimpl->s_InitialSpeedPPS = speedElement->FloatAttribute("Initial");
		pimpl->s_NormalSpeedPPS = speedElement->FloatAttribute("Normal");

		//Grab the data of position.
		auto positionElement = xmlElement->FirstChildElement("Position");
		auto initialOffset = positionElement->IntAttribute("MaxInitialOffset");
		pimpl->s_RandomPositionOffset.param({ -initialOffset, initialOffset });

		//Grab the data of color.
		auto colorElement = xmlElement->FirstChildElement("Color");
		for (auto concreteColor = colorElement->FirstChildElement(); concreteColor; concreteColor = concreteColor->NextSiblingElement()){
			auto colorType = concreteColor->Value();
			auto textureName = concreteColor->Attribute("TextureName");
			auto colorR = concreteColor->FloatAttribute("R");
			auto colorG = concreteColor->FloatAttribute("G");
			auto colorB = concreteColor->FloatAttribute("B");

			auto colorStruct = StarScriptImpl::ColorStruct();
			colorStruct.typeName = colorType;
			colorStruct.textureName = textureName;
			colorStruct.color4F = { colorR / 255.0f, colorG / 255.0f, colorB / 255.0f, 1.0f };
			pimpl->s_Colors.emplace_back(std::move(colorStruct));
		}
		pimpl->s_RandomColorNum.param({ 0, static_cast<int>(pimpl->s_Colors.size() - 1) });
	}

	return true;
}

void StarScript::vPostInit()
{
	auto actor = m_Actor.lock();

	pimpl->m_sprite = static_cast<cocos2d::Sprite*>(actor->getRenderComponent()->getSceneNode());

	pimpl->m_invoker = actor->getComponent<SequentialInvoker>().get();
	pimpl->m_invoker->setInvokeContinuously(true);
}

const std::string StarScript::Type = "StarScript";
