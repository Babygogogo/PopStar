#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "LevelLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataLevelStarted.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of LevelLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct LevelLabelScript::LevelLabelScriptImpl
{
	LevelLabelScriptImpl();
	~LevelLabelScriptImpl();

	void onLevelStarted(const IEventData & e);

	void setStringWithLevel(int levelIndex);

	static std::string s_LevelString;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string LevelLabelScript::LevelLabelScriptImpl::s_LevelString;

LevelLabelScript::LevelLabelScriptImpl::LevelLabelScriptImpl()
{
}

LevelLabelScript::LevelLabelScriptImpl::~LevelLabelScriptImpl()
{
}

void LevelLabelScript::LevelLabelScriptImpl::onLevelStarted(const IEventData & e)
{
	auto & levelStartedEvent = static_cast<const EvtDataLevelStarted &>(e);
	setStringWithLevel(levelStartedEvent.getLevelIndex());
}

void LevelLabelScript::LevelLabelScriptImpl::setStringWithLevel(int levelIndex)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_RenderComponent.lock()->getSceneNode());
	underlyingLabel->setString(s_LevelString + std::to_string(levelIndex));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of LevelLabelScript.
//////////////////////////////////////////////////////////////////////////
LevelLabelScript::LevelLabelScript() : pimpl{ std::make_shared<LevelLabelScriptImpl>() }
{
}

LevelLabelScript::~LevelLabelScript()
{
}

bool LevelLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto textElement = xmlElement->FirstChildElement("Text");
	pimpl->s_LevelString = textElement->Attribute("Level");

	isStaticInitialized = true;
	return true;
}

void LevelLabelScript::vPostInit()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::LevelStarted, pimpl, [this](const IEventData & e){
		pimpl->onLevelStarted(e);
	});

	pimpl->m_RenderComponent = m_Actor.lock()->getRenderComponent();
	pimpl->setStringWithLevel(0);
}

const std::string & LevelLabelScript::getType() const
{
	return Type;
}

const std::string LevelLabelScript::Type = "LevelLabelScript";
