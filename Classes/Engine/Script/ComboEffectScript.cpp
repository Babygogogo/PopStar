#include "ComboEffectScript.h"
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
struct ComboEffectScript::ComboEffectImpl
{
	ComboEffectImpl(ComboEffectScript *comboEffect);
	ComboEffectImpl(Actor *game_object, ComboEffectScript *comboEffect);
	~ComboEffectImpl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	std::string getTextureName(int explode_stars_num);
	void resetSpritePosition();
	void resetInvoker();

	void show(int explode_stars_num);

	cocos2d::Sprite * getSprite() const;
	SequentialInvoker * getSequentialInvoker() const;

	ComboEffectScript *m_Visitor{ nullptr };
};

ComboEffectScript::ComboEffectImpl::ComboEffectImpl(ComboEffectScript *comboEffect) : m_Visitor(comboEffect)
{
	registerAsEventListeners();
}

ComboEffectScript::ComboEffectImpl::ComboEffectImpl(Actor *game_object, ComboEffectScript *comboEffect) : m_Visitor(comboEffect)
{
	registerAsEventListeners();
}

ComboEffectScript::ComboEffectImpl::~ComboEffectImpl()
{
	unregisterAsEventListeners();
}

void ComboEffectScript::ComboEffectImpl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(LegacyEventType::UserClickedStarsExploded, this,
		[this](LegacyEvent *e){show(static_cast<EventArg1*>(e->getArg())->getInt()); });
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

	return {};
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
	return m_Visitor->m_Actor.lock()->getComponent<SequentialInvoker>();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of ComboEffect.
//////////////////////////////////////////////////////////////////////////
ComboEffectScript::ComboEffectScript() : BaseScriptComponent(), pimpl(new ComboEffectImpl(this))
{

}

ComboEffectScript::ComboEffectScript(Actor *game_object) :BaseScriptComponent("ComboEffect", game_object), pimpl(new ComboEffectImpl(game_object, this))
{

}

ComboEffectScript::~ComboEffectScript()
{

}

std::unique_ptr<ComboEffectScript> ComboEffectScript::create()
{
	return std::unique_ptr<ComboEffectScript>(new ComboEffectScript());
}

const std::string & ComboEffectScript::getType() const
{
	return Type;
}

const std::string ComboEffectScript::Type = "ComboEffectScript";
