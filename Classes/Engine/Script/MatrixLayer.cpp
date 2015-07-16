#include "MatrixLayer.h"
#include "StarMatrix.h"
#include "ComboEffect.h"
#include "../Utilities/SingletonContainer.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
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

//	Actor *m_matrix{ nullptr };
	std::weak_ptr<Actor> m_matrix;
};

MatrixLayer::impl::impl(Actor *game_object)
{
	game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	m_matrix = game_object->addChild(Actor::create<StarMatrix>("StarMatrix"));

	//Create the ComboEffect actor. Should be replaced by calls to factory soon...
//	game_object->addChild(Actor::create<ComboEffect>("ComboEffect"));
	auto comboEffect = Actor::create();
	comboEffect->addComponent<ComboEffect>();
	comboEffect->addComponent<DisplayNode>()->initAs<cocos2d::Sprite>();
	comboEffect->addComponent<SequentialInvoker>();
	game_object->addChild(std::move(comboEffect));

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
