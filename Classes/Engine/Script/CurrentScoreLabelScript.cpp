#include <string>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "CurrentScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataCurrentScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of CurrentScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct CurrentScoreLabelScript::CurrentScoreLabelScriptImpl
{
	CurrentScoreLabelScriptImpl();
	~CurrentScoreLabelScriptImpl();

	void setStringWithScore(int currentScore);
	void onCurrentScoreValueUpdated(const IEventData & e);

	static std::string s_TextPrefix;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::s_TextPrefix;

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::CurrentScoreLabelScriptImpl()
{
}

CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::~CurrentScoreLabelScriptImpl()
{
}

void CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::setStringWithScore(int currentScore)
{
	auto scoreText = s_TextPrefix + std::to_string(currentScore);
	static_cast<cocos2d::Label*>(m_RenderComponent.lock()->getSceneNode())->setString(scoreText);
}

void CurrentScoreLabelScript::CurrentScoreLabelScriptImpl::onCurrentScoreValueUpdated(const IEventData & e)
{
	auto newCurrentScore = (static_cast<const EvtDataCurrentScoreUpdated &>(e)).getNewCurrentScore();
	setStringWithScore(newCurrentScore);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of CurrentScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
CurrentScoreLabelScript::CurrentScoreLabelScript() : pimpl{ std::make_shared<CurrentScoreLabelScriptImpl>() }
{
}

CurrentScoreLabelScript::~CurrentScoreLabelScript()
{
}

bool CurrentScoreLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	auto s_IsStaticInitialized = false;
	if (s_IsStaticInitialized)
		return true;

	auto textElement = xmlElement->FirstChildElement("Text");
	pimpl->s_TextPrefix = textElement->Attribute("Prefix");

	s_IsStaticInitialized = true;
	return true;
}

void CurrentScoreLabelScript::vPostInit()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::CurrentScoreUpdated, pimpl, [this](const IEventData & e){
		pimpl->onCurrentScoreValueUpdated(e);
	});

	pimpl->m_RenderComponent = m_Actor.lock()->getRenderComponent();
	pimpl->setStringWithScore(0);
}

const std::string & CurrentScoreLabelScript::getType() const
{
	return Type;
}

const std::string CurrentScoreLabelScript::Type = "CurrentScoreLabelScript";
