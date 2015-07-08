#include "MatrixLayer.h"
#include "StarMatrix.h"
#include "ComboEffect.h"
#include "../Engine/Utilities/SingletonContainer.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Engine/Event/EventType.h"
#include "../Engine/Event/EventDispatcher.h"

#include "./Classes/Audio.h"

struct MatrixLayer::impl
{
	impl(GameObject *game_object);
	~impl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void startLevel();

	GameObject *m_matrix{ nullptr };
};

MatrixLayer::impl::impl(GameObject *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	m_matrix = game_object->addChild(GameObject::create<StarMatrix>("StarMatrix"));
	game_object->addChild(GameObject::create<ComboEffect>("ComboEffect"));

	registerAsEventListeners();
}

MatrixLayer::impl::~impl()
{
	unregisterAsEventListeners();
}

void MatrixLayer::impl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<EventDispatcher>()->registerListener(LegacyEventType::StartLevelLabelDisappeared, this,
		[this](LegacyEvent *){startLevel(); });
}

void MatrixLayer::impl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void MatrixLayer::impl::startLevel()
{
	m_matrix->getComponent<StarMatrix>()->reset();
	Audio::getInstance()->playReadyGo();
}

MatrixLayer::MatrixLayer(GameObject *game_object) :Script("MatrixLayer", game_object), pimpl(new impl(game_object))
{

}

MatrixLayer::~MatrixLayer()
{

}
