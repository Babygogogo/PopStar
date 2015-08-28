#include "cocos2d.h"

#include "MatrixLayerScript.h"
#include "StarMatrixScript.h"
#include "../Utilities/SingletonContainer.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
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

	void onStartLevelLabelDisappeared();

	std::weak_ptr<Actor> m_matrix;
};

MatrixLayerScript::MatrixLayerScriptImpl::MatrixLayerScriptImpl()
{
}

MatrixLayerScript::MatrixLayerScriptImpl::~MatrixLayerScriptImpl()
{
}

void MatrixLayerScript::MatrixLayerScriptImpl::onStartLevelLabelDisappeared()
{
	m_matrix.lock()->getComponent<StarMatrixScript>()->reset();
	Audio::getInstance()->playReadyGo();
}

//////////////////////////////////////////////////////////////////////////
//Implementation of MatrixLayerScript.
//////////////////////////////////////////////////////////////////////////
MatrixLayerScript::MatrixLayerScript() : pimpl{ std::make_unique<MatrixLayerScriptImpl>() }
{
}

MatrixLayerScript::~MatrixLayerScript()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

const std::string & MatrixLayerScript::getType() const
{
	return Type;
}

void MatrixLayerScript::vPostInit()
{
	auto actor = m_Actor.lock();

	auto starMatrixActor = SingletonContainer::getInstance()->get<GameLogic>()->createActor("Actors\\StarMatrix.xml");
	actor->addChild(starMatrixActor);
	pimpl->m_matrix = starMatrixActor;

	//Register as EventListener.
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::StartLevelLabelDisappeared, this,
		[this](BaseEventData *){pimpl->onStartLevelLabelDisappeared(); });
}

const std::string MatrixLayerScript::Type = "MatrixLayerScript";
