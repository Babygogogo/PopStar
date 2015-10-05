#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "GetScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataPlayerGotScore.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
struct GetScoreLabelScript::GetScoreLabelScriptImpl
{
	GetScoreLabelScriptImpl();
	~GetScoreLabelScriptImpl();

	void onPlayerGotScore(const IEventData & e);
	void onLevelSummaryFinished(const IEventData & e);

	static std::string s_ExplodedStarsString;
	static std::string s_GetScoreString;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string GetScoreLabelScript::GetScoreLabelScriptImpl::s_ExplodedStarsString;
std::string GetScoreLabelScript::GetScoreLabelScriptImpl::s_GetScoreString;

GetScoreLabelScript::GetScoreLabelScriptImpl::GetScoreLabelScriptImpl()
{
}

GetScoreLabelScript::GetScoreLabelScriptImpl::~GetScoreLabelScriptImpl()
{
}

void GetScoreLabelScript::GetScoreLabelScriptImpl::onPlayerGotScore(const IEventData & e)
{
	auto & playerGotScoreEvent = static_cast<const EvtDataPlayerGotScore &>(e);
	auto explodedStarsCountString = s_ExplodedStarsString + std::to_string(playerGotScoreEvent.getExplodedStarsCount());
	auto gotScore = s_GetScoreString + std::to_string(playerGotScoreEvent.getScore());

	auto underlyingLabel = static_cast<cocos2d::Label*>(m_RenderComponent.lock()->getSceneNode());
	underlyingLabel->setString(explodedStarsCountString + gotScore);
	underlyingLabel->setVisible(true);
}

void GetScoreLabelScript::GetScoreLabelScriptImpl::onLevelSummaryFinished(const IEventData & e)
{
	m_RenderComponent.lock()->getSceneNode()->setVisible(false);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
GetScoreLabelScript::GetScoreLabelScript() : pimpl{ std::make_shared<GetScoreLabelScriptImpl>() }
{
}

GetScoreLabelScript::~GetScoreLabelScript()
{
}

bool GetScoreLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto textElement = xmlElement->FirstChildElement("Text");
	GetScoreLabelScriptImpl::s_ExplodedStarsString = textElement->Attribute("ExplodedStars");
	GetScoreLabelScriptImpl::s_GetScoreString = textElement->Attribute("GetScore");

	isStaticInitialized = true;
	return true;
}

void GetScoreLabelScript::vPostInit()
{
	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EventType::PlayerGotScore, pimpl, [this](const IEventData & e){
		pimpl->onPlayerGotScore(e);
	});
	eventDispatcher->vAddListener(EventType::LevelSummaryFinished, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryFinished(e);
	});

	pimpl->m_RenderComponent = m_Actor.lock()->getRenderComponent();
}

const std::string & GetScoreLabelScript::getType() const
{
	return Type;
}

const std::string GetScoreLabelScript::Type = "GetScoreLabelScript";
