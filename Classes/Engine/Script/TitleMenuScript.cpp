#include <vector>

#include "TitleMenuScript.h"
#include "../Actor/Actor.h"
#include "../GameLogic/GameLogic.h"
#include "../Utilities/SingletonContainer.h"
#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of TitleMenuScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct TitleMenuScript::TitleMenuScriptImpl
{
	TitleMenuScriptImpl();
	~TitleMenuScriptImpl();

	std::vector<std::shared_ptr<Actor>> m_Composites;
};

TitleMenuScript::TitleMenuScriptImpl::TitleMenuScriptImpl()
{
}

TitleMenuScript::TitleMenuScriptImpl::~TitleMenuScriptImpl()
{
}

//////////////////////////////////////////////////////////////////////////
//Implementation of TitleMenuScript.
//////////////////////////////////////////////////////////////////////////
TitleMenuScript::TitleMenuScript() : pimpl{ std::make_unique<TitleMenuScriptImpl>() }
{
}

TitleMenuScript::~TitleMenuScript()
{
}

bool TitleMenuScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	auto compositeList = xmlElement->FirstChildElement("Composites");

	for (auto composite = compositeList->FirstAttribute(); composite; composite = composite->Next())
		pimpl->m_Composites.emplace_back(gameLogic->createActor(composite->Value()));

	return true;
}

void TitleMenuScript::vPostInit()
{
	auto actor = m_Actor.lock();

	for (auto && composite : pimpl->m_Composites)
		actor->addChild(std::move(composite));
	pimpl->m_Composites.clear();
}

const std::string & TitleMenuScript::getType() const
{
	return Type;
}

const std::string TitleMenuScript::Type = "TitleMenuScript";
