#include <string>
#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "ComboEffectScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"
#include "../Event/BaseEventData.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "../Event/IEventDispatcher.h"
#include "../Utilities/SingletonContainer.h"
#include "../Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ComboEffectImpl.
//////////////////////////////////////////////////////////////////////////
struct ComboEffectScript::ComboEffectScriptImpl
{
	ComboEffectScriptImpl();
	~ComboEffectScriptImpl();

	void onPlayerExplodedStars(const IEventData & e);

	std::string _getSpriteFrameName(int explodedStarsCount) const;

	static std::vector<std::pair<int, std::string>> s_ConditionalSpriteFrameNameList;
	static float s_BlinkDuration;
	static int s_BlinkCount;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
	std::weak_ptr<FiniteTimeActionComponent> m_FiniteTimeActionComponent;
};

std::vector<std::pair<int, std::string>> ComboEffectScript::ComboEffectScriptImpl::s_ConditionalSpriteFrameNameList;
float ComboEffectScript::ComboEffectScriptImpl::s_BlinkDuration{ 0.0f };
int ComboEffectScript::ComboEffectScriptImpl::s_BlinkCount{ 0 };

ComboEffectScript::ComboEffectScriptImpl::ComboEffectScriptImpl()
{
}

ComboEffectScript::ComboEffectScriptImpl::~ComboEffectScriptImpl()
{
}

void ComboEffectScript::ComboEffectScriptImpl::onPlayerExplodedStars(const IEventData & e)
{
	auto explodedStarsCount = (static_cast<const EvtDataPlayerExplodedStars &>(e)).getExplodedStarsCount();

	//Get the sprite frame name corresponding to explodedStarsCount.
	auto spriteFrameName = _getSpriteFrameName(explodedStarsCount);
	if (spriteFrameName.empty())
		return;

	//Get the underlying sprite and update its appearance.
	auto underlyingSprite = static_cast<cocos2d::Sprite*>(m_RenderComponent.lock()->getSceneNode());
	underlyingSprite->setSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName));
	underlyingSprite->setVisible(true);

	//Make the sprite blink and then disappear.
	auto renderComponent = m_RenderComponent;
	auto finiteTimeActionComponent = m_FiniteTimeActionComponent.lock();
	finiteTimeActionComponent->stopAndClearAllActions();
	finiteTimeActionComponent->queueBlink(s_BlinkDuration, s_BlinkCount, [renderComponent]{
		renderComponent.lock()->getSceneNode()->setVisible(false);
	});
	finiteTimeActionComponent->runNextAction();

	//Play sound effect.
	Audio::getInstance()->playCombo(explodedStarsCount);
}

std::string ComboEffectScript::ComboEffectScriptImpl::_getSpriteFrameName(int explodedStarsCount) const
{
	for (const auto & conditionSpriteFrameName : s_ConditionalSpriteFrameNameList)
		if (explodedStarsCount >= conditionSpriteFrameName.first)
			return conditionSpriteFrameName.second;

	return{};
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ComboEffect.
//////////////////////////////////////////////////////////////////////////
ComboEffectScript::ComboEffectScript() : pimpl{ std::make_shared<ComboEffectScriptImpl>() }
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::PlayerExplodedStars, pimpl, [this](const IEventData & e){
		pimpl->onPlayerExplodedStars(e);
	});
}

ComboEffectScript::~ComboEffectScript()
{
}

bool ComboEffectScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto s_IsStaticInitialized = false;
	if (s_IsStaticInitialized)
		return true;

	//Initialize the conditional sprite frame name list.
	auto spriteFrameNamesElement = xmlElement->FirstChildElement("ConditionalSpriteFrameNames");
	for (auto conditionElement = spriteFrameNamesElement->FirstChildElement("Condition"); conditionElement; conditionElement = conditionElement->NextSiblingElement()){
		auto conditionValue = conditionElement->IntAttribute("GreaterThanOrEqualTo");
		auto spriteFrameName = std::string(conditionElement->Attribute("SpriteFrameName"));

		pimpl->s_ConditionalSpriteFrameNameList.emplace_back(std::make_pair(std::move(conditionValue), std::move(spriteFrameName)));
	}

	//Initialize the data of blink.
	auto blinkElement = xmlElement->FirstChildElement("Blink");
	pimpl->s_BlinkDuration = blinkElement->FloatAttribute("Duration");
	pimpl->s_BlinkCount = blinkElement->IntAttribute("Count");

	s_IsStaticInitialized = true;
	return true;
}

const std::string & ComboEffectScript::getType() const
{
	return Type;
}

void ComboEffectScript::vPostInit()
{
	auto strongActor = m_Actor.lock();
	pimpl->m_RenderComponent = strongActor->getRenderComponent();
	pimpl->m_FiniteTimeActionComponent = strongActor->getComponent<FiniteTimeActionComponent>();
}

const std::string ComboEffectScript::Type = "ComboEffectScript";
