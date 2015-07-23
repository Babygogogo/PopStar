#include "ComboEffectScript.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../Event/EventDispatcher.h"
#include "../Event/BaseEventData.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "cocos2d.h"
#include "../Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ComboEffectImpl.
//////////////////////////////////////////////////////////////////////////
struct ComboEffectScript::ComboEffectImpl
{
	ComboEffectImpl(ComboEffectScript *visitor);
	~ComboEffectImpl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	std::string getTextureName(int explode_stars_num);
	void resetSpritePosition();
	void resetInvoker();

	void show(int explode_stars_num);

	cocos2d::Sprite * getSprite() const;
	SequentialInvoker * getSequentialInvoker() const;

	//A back pointer of the ComboEffectScript object.
	//It will be safer to use std::weak_ptr, but it seems to be overkill by now.
	ComboEffectScript *m_Visitor{ nullptr };
};

ComboEffectScript::ComboEffectImpl::ComboEffectImpl(ComboEffectScript *visitor) : m_Visitor(visitor)
{
	registerAsEventListeners();
}

ComboEffectScript::ComboEffectImpl::~ComboEffectImpl()
{
	unregisterAsEventListeners();
}

void ComboEffectScript::ComboEffectImpl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::PlayerExplodedStars, this,
		//[this](LegacyEvent *e){show(static_cast<EventArg1*>(e->getArg())->getInt()); });
		[this](BaseEventData *e){show(dynamic_cast<EvtDataPlayerExplodedStars*>(e)->getExplodedStarsCount()); });
}

void ComboEffectScript::ComboEffectImpl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
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

void ComboEffectScript::ComboEffectImpl::resetSpritePosition()
{
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	getSprite()->setPosition(visible_size.width / 2, visible_size.height / 2);
}

void ComboEffectScript::ComboEffectImpl::show(int explode_stars_num)
{
	auto texture_name = getTextureName(explode_stars_num);
	if (texture_name.empty())
		return;

	getSprite()->setTexture(texture_name);
	resetSpritePosition();
	getSprite()->setVisible(true);

	resetInvoker();
	getSequentialInvoker()->invoke();

	Audio::getInstance()->playCombo(explode_stars_num);
}

void ComboEffectScript::ComboEffectImpl::resetInvoker()
{
	getSequentialInvoker()->addFiniteTimeAction(cocos2d::Sequence::create(
		cocos2d::Blink::create(1.0f, 5), cocos2d::CallFunc::create([this]{getSprite()->setVisible(false); }), nullptr));
}

cocos2d::Sprite * ComboEffectScript::ComboEffectImpl::getSprite() const
{
	return m_Visitor->m_Actor.lock()->getComponent<DisplayNode>()->getAs<cocos2d::Sprite>();
}

SequentialInvoker * ComboEffectScript::ComboEffectImpl::getSequentialInvoker() const
{
	return m_Visitor->m_Actor.lock()->getComponent<SequentialInvoker>().get();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ComboEffect.
//////////////////////////////////////////////////////////////////////////
ComboEffectScript::ComboEffectScript() : pimpl{ std::make_unique<ComboEffectImpl>(this) }
{
}

ComboEffectScript::~ComboEffectScript()
{
}

const std::string & ComboEffectScript::getType() const
{
	return Type;
}

bool ComboEffectScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	//#TODO: read data from xmlElement and avoid hard-coding the logic and resources.
	return true;
}

const std::string ComboEffectScript::Type = "ComboEffectScript";
