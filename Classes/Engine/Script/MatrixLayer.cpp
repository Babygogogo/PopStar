#include "MatrixLayer.h"
#include "StarMatrix.h"
#include "ComboEffect.h"
#include "../Utilities/SingletonContainer.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"

#include "../Audio/Audio.h"

struct MatrixLayer::impl
{
	impl(Actor *game_object);
	~impl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void startLevel();

	Actor *m_matrix{ nullptr };
};

MatrixLayer::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	m_matrix = game_object->addChild(Actor::create<StarMatrix>("StarMatrix"));
	game_object->addChild(Actor::create<ComboEffect>("ComboEffect"));

	registerAsEventListeners();
}

MatrixLayer::impl::~impl()
{
	unregisterAsEventListeners();
}

void MatrixLayer::impl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(LegacyEventType::StartLevelLabelDisappeared, this,
		[this](LegacyEvent *){startLevel(); });
}

void MatrixLayer::impl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void MatrixLayer::impl::startLevel()
{
	m_matrix->getComponent<StarMatrix>()->reset();
	Audio::getInstance()->playReadyGo();
}

MatrixLayer::MatrixLayer(Actor *game_object) :BaseScriptComponent("MatrixLayer", game_object), pimpl(new impl(game_object))
{

}

MatrixLayer::~MatrixLayer()
{

}
