#include <unordered_set>
#include <cassert>

#include "GeneralRenderComponent.h"
#include "Actor.h"
#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of DisplayNodeImpl.
//////////////////////////////////////////////////////////////////////////
struct GeneralRenderComponent::DisplayNodeImpl
{
public:
	DisplayNodeImpl();
	~DisplayNodeImpl();

	cocos2d::Sprite * createSprite(tinyxml2::XMLElement * xmlElement);
	cocos2d::Layer * createLayer(tinyxml2::XMLElement * xmlElement);
	cocos2d::Scene * createScene(tinyxml2::XMLElement * xmlElement);
	cocos2d::Label * createLabel(tinyxml2::XMLElement * xmlElement);
	cocos2d::ParticleExplosion * createParticleExplosion(tinyxml2::XMLElement * xmlElement);

	GeneralRenderComponent* m_parent{ nullptr };
	std::unordered_set<GeneralRenderComponent*> m_children;
};

GeneralRenderComponent::DisplayNodeImpl::DisplayNodeImpl()
{
}

GeneralRenderComponent::DisplayNodeImpl::~DisplayNodeImpl()
{
}

cocos2d::Sprite * GeneralRenderComponent::DisplayNodeImpl::createSprite(tinyxml2::XMLElement * xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")){
		if (auto fileName = createWith->Attribute("FileName"))
			return cocos2d::Sprite::create(fileName);
	}

	return cocos2d::Sprite::create();
}

cocos2d::Layer * GeneralRenderComponent::DisplayNodeImpl::createLayer(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Layer::create();
}

cocos2d::Scene * GeneralRenderComponent::DisplayNodeImpl::createScene(tinyxml2::XMLElement * xmlElement)
{
	return cocos2d::Scene::create();
}

cocos2d::Label * GeneralRenderComponent::DisplayNodeImpl::createLabel(tinyxml2::XMLElement * xmlElement)
{
	if (auto createWith = xmlElement->FirstChildElement("CreateWith")){
		if (createWith->Attribute("FunctionName", "createWithSystemFont")){
			auto text = createWith->Attribute("Text");
			auto fontName = createWith->Attribute("FontName");
			auto fontSize = createWith->FloatAttribute("FontSize");
			return cocos2d::Label::createWithSystemFont(text, fontName, fontSize);
		}
	}
	
	return cocos2d::Label::create();
}

cocos2d::ParticleExplosion * GeneralRenderComponent::DisplayNodeImpl::createParticleExplosion(tinyxml2::XMLElement * xmlElement)
{
	auto particle = cocos2d::ParticleExplosion::create();
	if (auto properties = xmlElement->FirstChildElement("Properties")){
		if (auto texture = properties->Attribute("Texture"))
			particle->setTexture(cocos2d::Director::getInstance()->getTextureCache()->addImage(texture));
		if (auto totalParticles = properties->IntAttribute("TotalParticles"))
			particle->setTotalParticles(totalParticles);
		if (auto startSize = properties->FloatAttribute("StartSize"))
			particle->setStartSize(startSize);
		if (auto life = properties->FloatAttribute("Life"))
			particle->setLife(life);
		if (auto speed = properties->FloatAttribute("Speed"))
			particle->setSpeed(speed);
		if (auto speedVariance = properties->FloatAttribute("SpeedVariance"))
			particle->setSpeedVar(speedVariance);

		if (auto startColorVariance = properties->FirstChildElement("StartColorVariance")){
			auto r = startColorVariance->FloatAttribute("R") / 255.0f;
			auto g = startColorVariance->FloatAttribute("G") / 255.0f;
			auto b = startColorVariance->FloatAttribute("B") / 255.0f;
			auto a = startColorVariance->FloatAttribute("A") / 255.0f;
			particle->setStartColorVar({ r, g, b, a });
		}
		if (auto endColorVariance = properties->FirstChildElement("EndColorVariance")){
			auto r = endColorVariance->FloatAttribute("R") / 255.0f;
			auto g = endColorVariance->FloatAttribute("G") / 255.0f;
			auto b = endColorVariance->FloatAttribute("B") / 255.0f;
			auto a = endColorVariance->FloatAttribute("A") / 255.0f;
			particle->setEndColorVar({ r, g, b, a });
		}
		if (auto gravity = properties->FirstChildElement("Gravity")){
			auto x = gravity->FloatAttribute("X");
			auto y = gravity->FloatAttribute("Y");
			particle->setGravity({ x, y });
		}
	}

	return particle;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of DisplayNode.
//////////////////////////////////////////////////////////////////////////
GeneralRenderComponent::GeneralRenderComponent() : pimpl{ std::make_unique<DisplayNodeImpl>() }
{
}

//GeneralRenderComponent::GeneralRenderComponent(Actor *game_object) : BaseRenderComponent("DisplayNode", game_object), pimpl{ std::make_unique<DisplayNodeImpl>() }
//{
//	if (auto parent_object = game_object->getParent())
//		parent_object->addComponent<GeneralRenderComponent>()->addChild(this);
//}

GeneralRenderComponent::GeneralRenderComponent(Actor *game_object) : pimpl{ std::make_unique<DisplayNodeImpl>() }
{
}

GeneralRenderComponent::~GeneralRenderComponent()
{
	for (auto &child : pimpl->m_children)
		child->pimpl->m_parent = nullptr;

	if (m_Node){
		m_Node->removeFromParent();
		m_Node->release();
	}

	if (pimpl->m_parent){
		pimpl->m_parent->pimpl->m_children.erase(this);
		pimpl->m_parent = nullptr;
	}
}

void GeneralRenderComponent::addChild(GeneralRenderComponent *child)
{
	if (!child || child->pimpl->m_parent)
		return;

	if (child->m_Node){
		if (this->m_Node)
			this->m_Node->addChild(child->m_Node);
		else
			this->initAs<cocos2d::Node>()->addChild(child->m_Node);
	}

	child->pimpl->m_parent = this;
	pimpl->m_children.emplace(child);
}

GeneralRenderComponent * GeneralRenderComponent::getParent() const
{
	return pimpl->m_parent;
}

void GeneralRenderComponent::removeFromParent()
{
	if (!pimpl->m_parent || !m_Node)
		return;

	pimpl->m_parent->pimpl->m_children.erase(this);
	pimpl->m_parent = nullptr;
	m_Node->removeFromParent();
}

const std::string & GeneralRenderComponent::getType() const
{
	return Type;
}

bool GeneralRenderComponent::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Get the type of node from xmlElement.
	auto nodeType = xmlElement->Attribute("Type");

	//Create the node as the type.
	//#TODO: Complete the if statements.
	//#TODO: Cache the frequently used xmls to avoid unnecessary disk i/o.
	if (strcmp(nodeType, "Sprite") == 0)
		m_Node = pimpl->createSprite(xmlElement);
	else if (strcmp(nodeType, "Layer") == 0)
		m_Node = pimpl->createLayer(xmlElement);
	else if (strcmp(nodeType, "Scene") == 0)
		m_Node = pimpl->createScene(xmlElement);
	else if (strcmp(nodeType, "Label") == 0)
		m_Node = pimpl->createLabel(xmlElement);
	else if (strcmp(nodeType, "ParticleExplosion") == 0)
		m_Node = pimpl->createParticleExplosion(xmlElement);

	//Ensure that the node is created, then retain it.
	assert(m_Node && "GeneralRenderComponent::vInit can't create a cocos2d::Node!");
	m_Node->retain();

	//Set some extra data if presents in the xmlElement.
	if (auto positionElement = xmlElement->FirstChildElement("Position")){
		auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

		auto normalizedX = positionElement->FloatAttribute("NormalizedX");
		auto pixelOffsetX = positionElement->FloatAttribute("PixelOffsetX");
		auto normalizedY = positionElement->FloatAttribute("NormalizedY");
		auto pixelOffsetY = positionElement->FloatAttribute("PixelOffsetY");
		m_Node->setPosition(visibleSize.width * normalizedX + pixelOffsetX, visibleSize.height * normalizedY + pixelOffsetY);

		auto localZOrder = positionElement->IntAttribute("LocalZOrder");
		m_Node->setLocalZOrder(localZOrder);
	}

	return true;
}

cocos2d::Node * GeneralRenderComponent::initAsHelper(std::function<void*()> && creatorFunction)
{
	//Ensure that the node hasn't been initialized.
	assert(!m_Node);

	m_Node = static_cast<cocos2d::Node*>(creatorFunction());
	m_Node->retain();

	//attach to parent
	if (auto parent = getParent()){
		if (parent->m_Node)
			parent->m_Node->addChild(this->m_Node);
		else
			parent->initAs<cocos2d::Node>()->addChild(this->m_Node);
	}

	return m_Node;
}

const std::string GeneralRenderComponent::Type = "GeneralRenderComponent";
