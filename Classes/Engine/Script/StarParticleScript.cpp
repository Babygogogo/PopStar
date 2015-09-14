#include "cocos2d.h"

#include "StarParticleScript.h"
#include "StarScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../Utilities/SingletonContainer.h"

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

	auto particle = static_cast<cocos2d::ParticleExplosion*>(actor->getRenderComponent()->getSceneNode());
	particle->setStartColor(star->getColor4F());
	particle->setEndColor(star->getColor4F());
	particle->setPosition(star->getPositionX(), star->getPositionY());

	actor->getComponent<FiniteTimeActionComponent>()->runNextAction();
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
	auto invoker = m_Actor.lock()->getComponent<FiniteTimeActionComponent>();
	invoker->queueAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(1.5f), cocos2d::CallFunc::create([this]{
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestDestoryActor>(m_Actor.lock()->getID()));
	}), nullptr));
}

const std::string StarParticleScript::Type = "StarParticleScript";
