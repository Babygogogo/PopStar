#include "cocos2d.h"

#include "MatrixLayerScript.h"
#include "StarMatrixScript.h"
#include "../Utilities/SingletonContainer.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"
#include "../GameLogic/GameLogic.h"
#include "../Audio/Audio.h"

//////////////////////////////////////////////////////////////////////////
//Definition of MatrixLayerScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct MatrixLayerScript::MatrixLayerScriptImpl
{
	MatrixLayerScriptImpl();
	~MatrixLayerScriptImpl();

	void onStartLevelLabelDisappeared(const IEventData & e);

	std::weak_ptr<Actor> m_matrix;
};

MatrixLayerScript::MatrixLayerScriptImpl::MatrixLayerScriptImpl()
{
}

MatrixLayerScript::MatrixLayerScriptImpl::~MatrixLayerScriptImpl()
{
}

void MatrixLayerScript::MatrixLayerScriptImpl::onStartLevelLabelDisappeared(const IEventData & e)
{
	m_matrix.lock()->getComponent<StarMatrixScript>()->reset();
	Audio::getInstance()->playReadyGo();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of MatrixLayerScript.
//////////////////////////////////////////////////////////////////////////
MatrixLayerScript::MatrixLayerScript() : pimpl{ std::make_shared<MatrixLayerScriptImpl>() }
{
}

MatrixLayerScript::~MatrixLayerScript()
{
}

void MatrixLayerScript::vPostInit()
{
	auto actor = m_Actor.lock();

	auto starMatrixActor = SingletonContainer::getInstance()->get<GameLogic>()->createActor("Actors\\StarMatrix.xml");
	actor->addChild(starMatrixActor);
	pimpl->m_matrix = starMatrixActor;

	//Register as EventListener.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::StartLevelLabelDisappeared, pimpl, [this](const IEventData & e){
		pimpl->onStartLevelLabelDisappeared(e);
	});
}

const std::string & MatrixLayerScript::getType() const
{
	return Type;
}

const std::string MatrixLayerScript::Type = "MatrixLayerScript";
