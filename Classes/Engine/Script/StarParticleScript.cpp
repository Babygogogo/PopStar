#include "StarParticleScript.h"
#include "StarScript.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../Utilities/SingletonContainer.h"

#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StarPatricleEffectScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StarParticleScript::StarParticleScriptImpl
{
	StarParticleScriptImpl();
	~StarParticleScriptImpl();
};

StarParticleScript::StarParticleScriptImpl::StarParticleScriptImpl()
{
}

StarParticleScript::StarParticleScriptImpl::~StarParticleScriptImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of StarParticleScript.
//////////////////////////////////////////////////////////////////////////
StarParticleScript::StarParticleScript() : pimpl{ std::make_unique<StarParticleScriptImpl>() }
{
}

StarParticleScript::~StarParticleScript()
{
}

void StarParticleScript::show(StarScript *star)
{
	if (!star)
		return;

	auto actor = m_Actor.lock();

	auto particle = actor->getComponent<GeneralRenderComponent>()->getAs<cocos2d::ParticleExplosion>();
	particle->setStartColor(star->getColor4F());
	particle->setEndColor(star->getColor4F());
	particle->setPosition(star->getPositionX(), star->getPositionY());

	actor->getComponent<SequentialInvoker>()->invoke();
}

const std::string & StarParticleScript::getType() const
{
	return Type;
}

bool StarParticleScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	return true;
}

void StarParticleScript::vPostInit()
{
	auto invoker = m_Actor.lock()->getComponent<SequentialInvoker>();
	invoker->addFiniteTimeAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1.5f), cocos2d::CallFunc::create([this]{
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestDestoryActor>(m_Actor.lock()->getID()));
	}), nullptr));
}

const std::string StarParticleScript::Type = "StarParticleScript";
