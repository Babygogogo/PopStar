#include "ComboEffect.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../Event/EventDispatcher.h"
#include "../Event/LegacyEvent.h"
#include "../Event/EventType.h"
#include "../Event/EventArg1.h"

#include "../Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////
//Definition of ComboEffectImpl.
//////////////////////////////////////////////////////////////////////////
struct ComboEffect::ComboEffectImpl
{
	ComboEffectImpl(ComboEffect *comboEffect);
	ComboEffectImpl(Actor *game_object, ComboEffect *comboEffect);
	~ComboEffectImpl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	std::string getTextureName(int explode_stars_num);
	void resetSpritePosition();
	void resetInvoker();

	void show(int explode_stars_num);

	cocos2d::Sprite * getSprite() const;
	SequentialInvoker * getSequentialInvoker() const;

	ComboEffect *m_Visitor{ nullptr };

	//cocos2d::Sprite *m_sprite{ nullptr };
	//SequentialInvoker *m_invoker{ nullptr };
};

ComboEffect::ComboEffectImpl::ComboEffectImpl(ComboEffect *comboEffect) : m_Visitor(comboEffect)
{
	registerAsEventListeners();
}

ComboEffect::ComboEffectImpl::ComboEffectImpl(Actor *game_object, ComboEffect *comboEffect) : m_Visitor(comboEffect)
{
	game_object->setNeedUpdate(false);

	//m_sprite = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	//m_invoker = game_object->addComponent<SequentialInvoker>();
	//game_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	//game_object->addComponent<SequentialInvoker>();

	registerAsEventListeners();
}

ComboEffect::ComboEffectImpl::~ComboEffectImpl()
{
	unregisterAsEventListeners();
}

void ComboEffect::ComboEffectImpl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(LegacyEventType::UserClickedStarsExploded, this,
		[this](LegacyEvent *e){show(static_cast<EventArg1*>(e->getArg())->getInt()); });
}

void ComboEffect::ComboEffectImpl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

std::string ComboEffect::ComboEffectImpl::getTextureName(int explode_stars_num)
{
	if (explode_stars_num >= 10)
		return "combo_3.png";
	else if (explode_stars_num >= 7)
		return "combo_2.png";
	else if (explode_stars_num >= 5)
		return "combo_1.png";

	return {};
}

void ComboEffect::ComboEffectImpl::resetSpritePosition()
{
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
//	m_sprite->setPosition(visible_size.width / 2, visible_size.height / 2);
	getSprite()->setPosition(visible_size.width / 2, visible_size.height / 2);
}

void ComboEffect::ComboEffectImpl::show(int explode_stars_num)
{
	auto texture_name = getTextureName(explode_stars_num);
	if (texture_name.empty())
		return;

	//m_sprite->setTexture(texture_name);
	getSprite()->setTexture(texture_name);
	resetSpritePosition();
//	m_sprite->setVisible(true);
	getSprite()->setVisible(true);

	resetInvoker();
	//m_invoker->invoke();
	getSequentialInvoker()->invoke();

	Audio::getInstance()->playCombo(explode_stars_num);
}

void ComboEffect::ComboEffectImpl::resetInvoker()
{
	//m_invoker->clear();
	//m_invoker->addFiniteTimeAction(cocos2d::Sequence::create(
	//	cocos2d::Blink::create(1.0f, 5), cocos2d::CallFunc::create([this]{m_sprite->setVisible(false); }), nullptr));
	getSequentialInvoker()->clear();
	getSequentialInvoker()->addFiniteTimeAction(cocos2d::Sequence::create(
		cocos2d::Blink::create(1.0f, 5), cocos2d::CallFunc::create([this]{getSprite()->setVisible(false); }), nullptr));
}

cocos2d::Sprite * ComboEffect::ComboEffectImpl::getSprite() const
{
	return m_Visitor->m_game_object->getComponent<DisplayNode>()->getAs<cocos2d::Sprite>();
}

SequentialInvoker * ComboEffect::ComboEffectImpl::getSequentialInvoker() const
{
	return m_Visitor->m_game_object->getComponent<SequentialInvoker>();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ComboEffect.
//////////////////////////////////////////////////////////////////////////
ComboEffect::ComboEffect() : BaseScriptComponent(), pimpl(new ComboEffectImpl(this))
{

}

ComboEffect::ComboEffect(Actor *game_object) :BaseScriptComponent("ComboEffect", game_object), pimpl(new ComboEffectImpl(game_object, this))
{

}

ComboEffect::~ComboEffect()
{

}

std::unique_ptr<ComboEffect> ComboEffect::create()
{
	return std::unique_ptr<ComboEffect>(new ComboEffect());
}

const std::string & ComboEffect::getType() const
{
	return Type;
}

const std::string ComboEffect::Type = "ComboEffect";
