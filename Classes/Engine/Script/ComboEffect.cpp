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

struct ComboEffect::impl
{
	impl(Actor *game_object);
	~impl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	std::string getTextureName(int explode_stars_num);
	void resetSpritePosition();
	void resetInvoker();

	void show(int explode_stars_num);

	cocos2d::Sprite *m_sprite{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
};

ComboEffect::impl::impl(Actor *game_object)
{
	game_object->setNeedUpdate(false);

	m_sprite = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	m_invoker = game_object->addComponent<SequentialInvoker>();

	registerAsEventListeners();
}

ComboEffect::impl::~impl()
{
	unregisterAsEventListeners();
}

void ComboEffect::impl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(LegacyEventType::UserClickedStarsExploded, this,
		[this](LegacyEvent *e){show(static_cast<EventArg1*>(e->getArg())->getInt()); });
}

void ComboEffect::impl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

std::string ComboEffect::impl::getTextureName(int explode_stars_num)
{
	if (explode_stars_num >= 10)
		return "combo_3.png";
	else if (explode_stars_num >= 7)
		return "combo_2.png";
	else if (explode_stars_num >= 5)
		return "combo_1.png";

	return {};
}

void ComboEffect::impl::resetSpritePosition()
{
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_sprite->setPosition(visible_size.width / 2, visible_size.height / 2);
}

void ComboEffect::impl::show(int explode_stars_num)
{
	auto texture_name = getTextureName(explode_stars_num);
	if (texture_name.empty())
		return;

	m_sprite->setTexture(texture_name);
	resetSpritePosition();
	m_sprite->setVisible(true);

	resetInvoker();
	m_invoker->invoke();

	Audio::getInstance()->playCombo(explode_stars_num);
}

void ComboEffect::impl::resetInvoker()
{
	m_invoker->clear();
	m_invoker->addFiniteTimeAction(cocos2d::Sequence::create(
		cocos2d::Blink::create(1.0f, 5), cocos2d::CallFunc::create([this]{m_sprite->setVisible(false); }), nullptr));
}

ComboEffect::ComboEffect(Actor *game_object) :BaseScriptComponent("ComboEffect", game_object), pimpl(new impl(game_object))
{

}

ComboEffect::~ComboEffect()
{

}
