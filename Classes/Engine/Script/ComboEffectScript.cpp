#include "cocos2d.h"

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
struct ComboEffectScript::ComboEffectImpl
{
	ComboEffectImpl(ComboEffectScript *visitor);
	~ComboEffectImpl();

	void onPlayerExplodedStars(const IEventData & e);

	std::string getTextureName(int explode_stars_num);

	//A back pointer of the ComboEffectScript object.
	//It will be safer to use std::weak_ptr, but it seems to be overkill by now.
	ComboEffectScript *m_Visitor{ nullptr };
};

ComboEffectScript::ComboEffectImpl::ComboEffectImpl(ComboEffectScript *visitor) : m_Visitor(visitor)
{
}

ComboEffectScript::ComboEffectImpl::~ComboEffectImpl()
{
}

void ComboEffectScript::ComboEffectImpl::onPlayerExplodedStars(const IEventData & e)
{
	auto explodedStarsCount = (static_cast<const EvtDataPlayerExplodedStars &>(e)).getExplodedStarsCount();

	//Display the combo effect. Firstly, get the texture name corresponding to explodedStarsCount.
	auto texture_name = getTextureName(explodedStarsCount);
	if (texture_name.empty())
		return;

	//Get the underlying sprite and update it.
	auto underlyingSprite = static_cast<cocos2d::Sprite*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode());
	//underlyingSprite->setTexture(texture_name);
	underlyingSprite->setSpriteFrame(cocos2d::SpriteFrameCache::getInstance()->getSpriteFrameByName(texture_name));
	underlyingSprite->setVisible(true);

	//Reset the sequential invoker so that the sprite will disappear as we wish
	auto sequentialInvoker = m_Visitor->m_Actor.lock()->getComponent<FiniteTimeActionComponent>();
	sequentialInvoker->queueAction(cocos2d::Sequence::create(
		cocos2d::Blink::create(1.0f, 5), cocos2d::CallFunc::create([underlyingSprite]{underlyingSprite->setVisible(false); }), nullptr));
	sequentialInvoker->runNextAction();

	//Play sound effect.
	Audio::getInstance()->playCombo(explodedStarsCount);
}

std::string ComboEffectScript::ComboEffectImpl::getTextureName(int explode_stars_num)
{
	if (explode_stars_num >= 10)
		return "combo_3.png";
	else if (explode_stars_num >= 7)
		return "combo_2.png";
	else if (explode_stars_num >= 5)
		return "combo_1.png";

	return{};
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ComboEffect.
//////////////////////////////////////////////////////////////////////////
ComboEffectScript::ComboEffectScript() : pimpl{ std::make_shared<ComboEffectImpl>(this) }
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
	//#TODO: read data from xmlElement and avoid hard-coding the logic and resources.
	return true;
}

const std::string & ComboEffectScript::getType() const
{
	return Type;
}

const std::string ComboEffectScript::Type = "ComboEffectScript";
