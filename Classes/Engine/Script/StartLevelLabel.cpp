#include "StartLevelLabel.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "cocos2d.h"

struct StartLevelLabel::impl
{
	impl(Actor *game_object);
	~impl();

	void registerAsEventListener();
	void unregisterAsEventListener();

	void reset();
	void resetInvoker();

	std::string createStartLevelText() const;

	cocos2d::Label *m_label_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
};

StartLevelLabel::impl::impl(Actor *game_object)
{
	m_label_underlying = game_object->addComponent<GeneralRenderComponent>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont("", "Verdana-Bold", 50); });
	m_label_underlying->setVisible(false);

	m_invoker = game_object->addComponent<SequentialInvoker>().get();

	registerAsEventListener();
	reset();
}

StartLevelLabel::impl::~impl()
{
	unregisterAsEventListener();
}

void StartLevelLabel::impl::registerAsEventListener()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::LevelStarted, this,
		[this](BaseEventData *){reset(); });

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		m_invoker->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, m_label_underlying);
}

void StartLevelLabel::impl::unregisterAsEventListener()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void StartLevelLabel::impl::reset()
{
	m_label_underlying->setString(createStartLevelText());
	m_label_underlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_label_underlying->setPosition(visible_size.width + m_label_underlying->getContentSize().width / 2, visible_size.height / 2);

	resetInvoker();
	m_invoker->invoke();
}

void StartLevelLabel::impl::resetInvoker()
{
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_invoker->addMoveTo(0.6f, visible_size.width / 2, visible_size.height / 2);
	m_invoker->addMoveTo(0.6f, -m_label_underlying->getContentSize().width / 2, visible_size.height / 2, [this]{
		m_label_underlying->setVisible(false);
		SingletonContainer::getInstance()->get<IEventDispatcher>()->dispatch(std::make_unique<EvtDataGeneric>(EventType::StartLevelLabelDisappeared));
	});
}

std::string StartLevelLabel::impl::createStartLevelText() const
{
	auto level_text = std::to_string(SingletonContainer::getInstance()->get<GameData>()->getCurrentLevel()) + '\n';
	return std::string("Level: ") + level_text + std::string(" Start!");
}

StartLevelLabel::StartLevelLabel(Actor *game_object) :BaseScriptComponent("StartLevelLabel", game_object), pimpl(new impl(game_object))
{
}

StartLevelLabel::~StartLevelLabel()
{
}

const std::string & StartLevelLabel::getType() const
{
	return Type;
}

const std::string StartLevelLabel::Type = "StartLevelLabelScript";
