#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "HighScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataHighScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of HighScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct HighScoreLabelScript::HighScoreLabelScriptImpl
{
	HighScoreLabelScriptImpl();
	~HighScoreLabelScriptImpl();

	void setStringWithScore(int highScore);

	void onHighScoreUpdated(const IEventData & e);

	static std::string s_HighScoreString;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string HighScoreLabelScript::HighScoreLabelScriptImpl::s_HighScoreString;

HighScoreLabelScript::HighScoreLabelScriptImpl::HighScoreLabelScriptImpl()
{
}

HighScoreLabelScript::HighScoreLabelScriptImpl::~HighScoreLabelScriptImpl()
{
}

void HighScoreLabelScript::HighScoreLabelScriptImpl::onHighScoreUpdated(const IEventData & e)
{
	const auto & highScoreEvent = static_cast<const EvtDataHighScoreUpdated &>(e);
	setStringWithScore(highScoreEvent.getHighScore());
}

void HighScoreLabelScript::HighScoreLabelScriptImpl::setStringWithScore(int highScore)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_RenderComponent.lock()->getSceneNode());
	underlyingLabel->setString(s_HighScoreString + std::to_string(highScore));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of HighScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
HighScoreLabelScript::HighScoreLabelScript() : pimpl{ std::make_shared<HighScoreLabelScriptImpl>() }
{
}

HighScoreLabelScript::~HighScoreLabelScript()
{
}

bool HighScoreLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto textElement = xmlElement->FirstChildElement("Text");
	pimpl->s_HighScoreString = textElement->Attribute("HighScore");

	isStaticInitialized = true;
	return true;
}

void HighScoreLabelScript::vPostInit()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::HighScoreUpdated, pimpl, [this](const IEventData & e){
		pimpl->onHighScoreUpdated(e);
	});

	pimpl->m_RenderComponent = m_Actor.lock()->getRenderComponent();
	pimpl->setStringWithScore(0);
}

const std::string & HighScoreLabelScript::getType() const
{
	return Type;
}

const std::string HighScoreLabelScript::Type = "HighScoreLabelScript";
