#include "MatrixLayer.h"
#include "StarMatrix.h"
#include "../Common/SingletonContainer.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"

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

	registerAsEventListeners();
}

MatrixLayer::impl::~impl()
{
	unregisterAsEventListeners();
}

void MatrixLayer::impl::registerAsEventListeners()
{
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::StartLevelLabelDisappeared, this,
		[this](Event *){startLevel(); });
}

void MatrixLayer::impl::unregisterAsEventListeners()
{
	if (auto singleton_container = SingletonContainer::instance())
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
