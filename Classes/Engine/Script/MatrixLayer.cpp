#include "MatrixLayer.h"
#include "StarMatrix.h"
#include "../Utilities/SingletonContainer.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"
#include "../GameLogic/GameLogic.h"
#include "cocos2d.h"
#include "../Audio/Audio.h"

struct MatrixLayer::impl
{
	impl(Actor *game_object);
	~impl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void startLevel();

	std::weak_ptr<Actor> m_matrix;
};

MatrixLayer::impl::impl(Actor *game_object)
{
	game_object->addComponent<GeneralRenderComponent>()->initAs<cocos2d::Layer>();

	auto starMatrixActor = std::make_shared<Actor>();
	starMatrixActor->addComponent<StarMatrix>();
	m_matrix = game_object->addChild(std::move(starMatrixActor));

	//Create the ComboEffect actor.
	auto comboEffect = SingletonContainer::getInstance()->get<GameLogic>()->createActor("Actors\\ComboEffect.xml");
	game_object->addChild(std::move(comboEffect));

	registerAsEventListeners();
}

MatrixLayer::impl::~impl()
{
	unregisterAsEventListeners();
}

void MatrixLayer::impl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::StartLevelLabelDisappeared, this,
		[this](BaseEventData *){startLevel(); });
}

void MatrixLayer::impl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void MatrixLayer::impl::startLevel()
{
	//m_matrix->getComponent<StarMatrix>()->reset();
	m_matrix.lock()->getComponent<StarMatrix>()->reset();
	Audio::getInstance()->playReadyGo();
}

MatrixLayer::MatrixLayer(Actor *game_object) :BaseScriptComponent("MatrixLayer", game_object), pimpl(new impl(game_object))
{
}

MatrixLayer::~MatrixLayer()
{
}

const std::string & MatrixLayer::getType() const
{
	return Type;
}

const std::string MatrixLayer::Type = "MatrixLayerScript";
