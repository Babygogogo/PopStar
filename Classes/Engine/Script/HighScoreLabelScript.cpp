#include "HighScoreLabelScript.h"
#include "cocos2d.h"
#include "../../Common/GameData.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Event/IEventDispatcher.h"
#include "../Event/EventType.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of HighScoreLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct HighScoreLabelScript::HighScoreLabelScriptImpl
{
	HighScoreLabelScriptImpl(HighScoreLabelScript *visitor);
	~HighScoreLabelScriptImpl();

	void setStringWithScore(int highScore = 0);

	void onHighScoreValueUpdated(const IEventData & e);

	HighScoreLabelScript *m_Visitor{ nullptr };
};

HighScoreLabelScript::HighScoreLabelScriptImpl::HighScoreLabelScriptImpl(HighScoreLabelScript *visitor) : m_Visitor{ visitor }
{
}

HighScoreLabelScript::HighScoreLabelScriptImpl::~HighScoreLabelScriptImpl()
{
}

void HighScoreLabelScript::HighScoreLabelScriptImpl::onHighScoreValueUpdated(const IEventData & e)
{
	setStringWithScore();
}

void HighScoreLabelScript::HighScoreLabelScriptImpl::setStringWithScore(int highScore /*= 0*/)
{
	auto underlyingLabel = static_cast<cocos2d::Label*>(m_Visitor->m_Actor.lock()->getRenderComponent()->getSceneNode());
	underlyingLabel->setString(std::string("High Score: ") + std::to_string(SingletonContainer::getInstance()->get<GameData>()->getHighScore()));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of HighScoreLabelScript.
//////////////////////////////////////////////////////////////////////////
HighScoreLabelScript::HighScoreLabelScript() : pimpl{ std::make_unique<HighScoreLabelScriptImpl>(this) }
{
}

HighScoreLabelScript::~HighScoreLabelScript()
{
}

void HighScoreLabelScript::vPostInit()
{
	pimpl->setStringWithScore();

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::HighScoreValueUpdated, pimpl, [this](const IEventData & e){
		pimpl->onHighScoreValueUpdated(e);
	});
}

const std::string & HighScoreLabelScript::getType() const
{
	return Type;
}

const std::string HighScoreLabelScript::Type = "HighScoreLabelScript";
