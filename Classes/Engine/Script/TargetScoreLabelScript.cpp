#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "TargetScoreLabelScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataTargetScoreUpdated.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TargetScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TargetScoreLabelScript::TargetScoreLabelScriptImpl
{
	TargetScoreLabelScriptImpl();
	~TargetScoreLabelScriptImpl();

	void onTargetScoreValueUpdated(const IEventData & e);

	void setStringWithTargetScore(int targetScore);

	static std::string s_TargetScoreString;

	std::weak_ptr<BaseRenderComponent> m_RenderComponent;
};

std::string TargetScoreLabelScript::TargetScoreLabelScriptImpl::s_TargetScoreString;

TargetScoreLabelScript::TargetScoreLabelScriptImpl::TargetScoreLabelScriptImpl()
{
}

TargetScoreLabelScript::TargetScoreLabelScriptImpl::~TargetScoreLabelScriptImpl()
{
}

void TargetScoreLabelScript::TargetScoreLabelScriptImpl::onTargetScoreValueUpdated(const IEventData & e)
{
	const auto & targetScoreEvent = static_cast<const EvtDataTargetScoreUpdated &>(e);
	setStringWithTargetScore(targetScoreEvent.getTargetScore());
}

void TargetScoreLabelScript::TargetScoreLabelScriptImpl::setStringWithTargetScore(int targetScore)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_RenderComponent.lock()->getSceneNode());
	underlyingLabel->setString(s_TargetScoreString + std::to_string(targetScore));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TargetScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
TargetScoreLabelScript::TargetScoreLabelScript() : pimpl{ std::make_shared<TargetScoreLabelScriptImpl>() }
{
}

TargetScoreLabelScript::~TargetScoreLabelScript()
{
}

bool TargetScoreLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

	auto textElement = xmlElement->FirstChildElement("Text");
	pimpl->s_TargetScoreString = textElement->Attribute("TargetScore");

	isStaticInitialized = true;
	return true;
}

void TargetScoreLabelScript::vPostInit()
{
	pimpl->m_RenderComponent = m_Actor.lock()->getRenderComponent();
	pimpl->setStringWithTargetScore(0);

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::TargetScoreUpdated, pimpl, [this](const IEventData & e){
		pimpl->onTargetScoreValueUpdated(e);
	});
}

const std::string & TargetScoreLabelScript::getType() const
{
	return Type;
}

const std::string TargetScoreLabelScript::Type = "TargetScoreLabelScript";
