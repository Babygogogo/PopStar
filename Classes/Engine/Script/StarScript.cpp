#include <random>
#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "StarScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StarScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StarScript::StarScriptImpl
{
	StarScriptImpl();
	~StarScriptImpl();

	void moveTo(float toPosX, float toPosY, float speed = StarScript::StarScriptImpl::s_NormalSpeedPPS);

	cocos2d::Sprite *m_UnderlyingSprite{ nullptr };
	std::weak_ptr<FiniteTimeActionComponent> m_ActionComponent;

	bool m_IsInGroup{ false };
	int m_RowIndex{ 0 }, m_ColIndex{ 0 };

	int m_ColorIndex{ 0 };

	static float s_InitialSpeedPPS;
	static float s_NormalSpeedPPS;

	struct ColorStruct{
		std::string typeName;
		std::string spriteFrameName;
		cocos2d::Color4F color4F;
	};
	static std::vector<ColorStruct> s_Colors;

	static std::random_device rd;
	static std::default_random_engine s_RandomEngine;
	static std::uniform_int_distribution<> s_RandomColorNum;
	static std::uniform_int_distribution<> s_RandomPositionOffset;
};

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

void StarScript::StarScriptImpl::moveTo(float toPosX, float toPosY, float speed /*= StarScript::StarScriptImpl::s_NormalSpeedPPS*/)
{
	auto currentPosX = m_UnderlyingSprite->getPositionX();
	auto currentPosY = m_UnderlyingSprite->getPositionY();
	if (toPosX == currentPosX && toPosY == currentPosY)
		return;

	//Calculate the time for moving.
	auto deltaX = currentPosX - toPosX;
	auto deltaY = currentPosY - toPosY;
	auto moveDuration = sqrt(deltaX * deltaX + deltaY * deltaY) / speed;

	auto actionComponent = m_ActionComponent.lock();
	actionComponent->stopAndClearAllActions();
	actionComponent->queueMoveTo(moveDuration, toPosX, toPosY);
	actionComponent->runNextAction();
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

void StarScript::randomizePositionAndColor(int rowIndex, int colIndex, float normalPosX, float normalPosY)
{
	//Randomize the color of the star.
	pimpl->m_ColorIndex = pimpl->s_RandomColorNum(pimpl->s_RandomEngine);
	pimpl->m_UnderlyingSprite->setSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(pimpl->s_Colors[pimpl->m_ColorIndex].spriteFrameName));
	pimpl->m_UnderlyingSprite->setVisible(true);

	//Randomize the position of the star, and move it to the normal position.
	auto randomPosX = normalPosX + pimpl->s_RandomPositionOffset(pimpl->s_RandomEngine);
	auto randomPosY = normalPosY + pimpl->s_RandomPositionOffset(pimpl->s_RandomEngine);
	pimpl->m_UnderlyingSprite->setPosition(randomPosX, randomPosY);
	pimpl->moveTo(normalPosX, normalPosY, StarScript::StarScriptImpl::s_InitialSpeedPPS);

	//Reset some data that are related to the matrix.
	pimpl->m_IsInGroup = false;
	pimpl->m_RowIndex = rowIndex;
	pimpl->m_ColIndex = colIndex;
}

void StarScript::moveTo(float posX, float posY)
{
	pimpl->moveTo(posX, posY);
}

bool StarScript::isInGroup() const
{
	return pimpl->m_IsInGroup;
}

void StarScript::setIsInGroup(bool isInGroup)
{
	pimpl->m_IsInGroup = isInGroup;
}

int StarScript::getRowIndex() const
{
	return pimpl->m_RowIndex;
}

void StarScript::setRowNum(int row_num)
{
	pimpl->m_RowIndex = row_num;
}

int StarScript::getColIndex() const
{
	return pimpl->m_ColIndex;
}

void StarScript::setColNum(int col_num)
{
	pimpl->m_ColIndex = col_num;
}

bool StarScript::canGroupWith(StarScript *star) const
{
	return (star) && (this != star) && (pimpl->m_ColorIndex == star->pimpl->m_ColorIndex);
}

cocos2d::Color4F StarScript::getColor4F() const
{
	return pimpl->s_Colors[pimpl->m_ColorIndex].color4F;
}

float StarScript::getPositionX() const
{
	return pimpl->m_UnderlyingSprite->getPositionX();
}

float StarScript::getPositionY() const
{
	return pimpl->m_UnderlyingSprite->getPositionY();
}

void StarScript::setVisible(bool visible)
{
	pimpl->m_UnderlyingSprite->setVisible(visible);
}

bool StarScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

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
		auto spriteFrameName = concreteColor->Attribute("SpriteFrameName");
		auto colorR = concreteColor->FloatAttribute("R");
		auto colorG = concreteColor->FloatAttribute("G");
		auto colorB = concreteColor->FloatAttribute("B");

		auto colorStruct = StarScriptImpl::ColorStruct();
		colorStruct.typeName = colorType;
		colorStruct.spriteFrameName = spriteFrameName;
		colorStruct.color4F = { colorR / 255.0f, colorG / 255.0f, colorB / 255.0f, 1.0f };
		pimpl->s_Colors.emplace_back(std::move(colorStruct));
	}
	pimpl->s_RandomColorNum.param({ 0, static_cast<int>(pimpl->s_Colors.size() - 1) });

	isStaticInitialized = true;
	return true;
}

void StarScript::vPostInit()
{
	auto actor = m_Actor.lock();
	pimpl->m_UnderlyingSprite = static_cast<cocos2d::Sprite*>(actor->getRenderComponent()->getSceneNode());
	pimpl->m_ActionComponent = actor->getComponent<FiniteTimeActionComponent>();
}

const std::string & StarScript::getType() const
{
	return Type;
}

const std::string StarScript::Type = "StarScript";
