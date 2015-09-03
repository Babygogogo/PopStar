#include "cocos2d.h"

#include "LevelLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataLevelIndexUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of LevelLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct LevelLabelScript::LevelLabelScriptImpl
{
	LevelLabelScriptImpl(LevelLabelScript *visitor);
	~LevelLabelScriptImpl();

	void onLevelValueUpdated(const IEventData & e);

	void setStringWithLevel(int levelValue = 0);

	LevelLabelScript *m_Visitor{ nullptr };
};

LevelLabelScript::LevelLabelScriptImpl::LevelLabelScriptImpl(LevelLabelScript *visitor) : m_Visitor{ visitor }
{
}

LevelLabelScript::LevelLabelScriptImpl::~LevelLabelScriptImpl()
{
}

void LevelLabelScript::LevelLabelScriptImpl::onLevelValueUpdated(const IEventData & e)
{
	auto & levelIndexEvent = static_cast<const EvtDataLevelIndexUpdated &>(e);
	setStringWithLevel(levelIndexEvent.getLevelIndex());
}

void LevelLabelScript::LevelLabelScriptImpl::setStringWithLevel(int levelValue /*= 0*/)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode());
	underlyingLabel->setString(std::string("Level: ") + std::to_string(levelValue));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of LevelLabelScript.
//////////////////////////////////////////////////////////////////////////
LevelLabelScript::LevelLabelScript() : pimpl{ std::make_shared<LevelLabelScriptImpl>(this) }
{
}

LevelLabelScript::~LevelLabelScript()
{
}

void LevelLabelScript::vPostInit()
{
	pimpl->setStringWithLevel();

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::LevelIndexUpdated, pimpl, [this](const IEventData & e){
		pimpl->onLevelValueUpdated(e);
	});
}

const std::string & LevelLabelScript::getType() const
{
	return Type;
}

const std::string LevelLabelScript::Type = "LevelLabelScript";
