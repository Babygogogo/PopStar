#include "StarParticleEffect.h"
#include "../Actor/Actor.h"
#include "StarScript.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "cocos2d.h"

struct StarParticleEffect::impl
{
	impl(Actor *game_object);
	~impl();

	void resetEffect(StarScript *star);
	void resetInvoker();

	Actor *m_game_object{ nullptr };
	cocos2d::ParticleExplosion *m_effect{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
};

StarParticleEffect::impl::impl(Actor *game_object) :m_game_object(game_object)
{
	m_effect = game_object->addComponent<GeneralRenderComponent>()->initAs<cocos2d::ParticleExplosion>();
	m_invoker = game_object->addComponent<SequentialInvoker>().get();
}

StarParticleEffect::impl::~impl()
{
}

void StarParticleEffect::impl::resetEffect(StarScript *star)
{
	m_effect->setTexture(cocos2d::Director::getInstance()->getTextureCache()->addImage("star.png"));
	m_effect->setTotalParticles(15);
	m_effect->setStartColor(star->getColor4F());
	m_effect->setStartColorVar({ 0, 0, 0, 1 });
	m_effect->setEndColor(star->getColor4F());
	m_effect->setEndColorVar({ 0, 0, 0, 1 });
	m_effect->setStartSize(25.0f);
	m_effect->setGravity({ 0, -300 });
	m_effect->setLife(1.0f);
	m_effect->setSpeed(200);
	m_effect->setSpeedVar(10);
	m_effect->setPosition(star->getPositionX(), star->getPositionY());
}

void StarParticleEffect::impl::resetInvoker()
{
	m_invoker->addFiniteTimeAction(cocos2d::Sequence::create(
		cocos2d::DelayTime::create(1.5f), cocos2d::CallFunc::create([this]{m_game_object->removeFromParent(); }), nullptr));
}

StarParticleEffect::StarParticleEffect(Actor *game_object) :BaseScriptComponent("StarParticleEffect", game_object), pimpl(new impl(game_object))
{
}

StarParticleEffect::~StarParticleEffect()
{
}

void StarParticleEffect::reset(StarScript *star)
{
	if (!star)
		return;

	pimpl->resetEffect(star);
	pimpl->resetInvoker();
	pimpl->m_invoker->invoke();
}

const std::string & StarParticleEffect::getType() const
{
	return Type;
}

const std::string StarParticleEffect::Type = "StarParticleEffectScript";
