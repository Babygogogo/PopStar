#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

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

	static float s_Life;
};

float StarParticleScript::StarParticleScriptImpl::s_Life{};

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

void StarParticleScript::show(float posX, float posY, const cocos2d::Color4F & color)
{
	auto actor = m_Actor.lock();

	auto particle = static_cast<cocos2d::ParticleExplosion*>(actor->getRenderComponent()->getSceneNode());
	particle->setLife(pimpl->s_Life);
	particle->setStartColor(color);
	particle->setEndColor(color);
	particle->setPosition(posX, posY);

	auto actionComponent = actor->getComponent<FiniteTimeActionComponent>();
	actionComponent->queueAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(pimpl->s_Life), cocos2d::CallFunc::create([this]{
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataRequestDestoryActor>(m_Actor.lock()->getID()));
	}), nullptr));
	actor->getComponent<FiniteTimeActionComponent>()->runNextAction();
}

bool StarParticleScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	pimpl->s_Life = xmlElement->FirstChildElement("Life")->FloatAttribute("Value");

	isStaticInitialized = true;
	return true;
}

const std::string & StarParticleScript::getType() const
{
	return Type;
}

const std::string StarParticleScript::Type = "StarParticleScript";
