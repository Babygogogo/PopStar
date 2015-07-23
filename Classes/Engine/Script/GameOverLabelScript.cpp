#include "GameOverLabelScript.h"
#include "TitleScene.h"
#include "../Actor/Actor.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
#include "../Utilities/SingletonContainer.h"
#include "../Graphic2D/SceneStack.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameOverLabelScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct GameOverLabelScript::GameOverLabelScriptImpl
{
	GameOverLabelScriptImpl();
	~GameOverLabelScriptImpl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void reset();
	void resetInvoker();

	cocos2d::Label *m_label_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };

	std::string m_GameOverText;
	std::string m_GameOverFontName;
	float m_GameOverFontSize{ 0 };
};

GameOverLabelScript::GameOverLabelScriptImpl::GameOverLabelScriptImpl()
{
	//registerAsEventListeners();
}

GameOverLabelScript::GameOverLabelScriptImpl::~GameOverLabelScriptImpl()
{
	unregisterAsEventListeners();
}

void GameOverLabelScript::GameOverLabelScriptImpl::registerAsEventListeners()
{
	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::GameOver, this,
		[this](BaseEventData *){reset(); });

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		m_invoker->invoke();
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, m_label_underlying);
}

void GameOverLabelScript::GameOverLabelScriptImpl::unregisterAsEventListeners()
{
	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void GameOverLabelScript::GameOverLabelScriptImpl::reset()
{
	m_label_underlying->setVisible(true);

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	auto label_size = m_label_underlying->getContentSize();
	m_label_underlying->setPosition(visible_size.width / 2, visible_size.height + label_size.height / 2);

	resetInvoker();
	m_invoker->invoke();
}

void GameOverLabelScript::GameOverLabelScriptImpl::resetInvoker()
{
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	m_invoker->addMoveTo(1, visible_size.width / 2, visible_size.height / 2);

	m_invoker->addCallback([]{
		auto titleSceneActor = std::make_shared<Actor>();
		titleSceneActor->addComponent<TitleScene>();
		SingletonContainer::getInstance()->get<SceneStack>()->replaceAndRun(std::move(titleSceneActor)); });
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameOverLabelScript.
//////////////////////////////////////////////////////////////////////////
GameOverLabelScript::GameOverLabelScript() : pimpl{ std::make_unique<GameOverLabelScriptImpl>() }
{
}

GameOverLabelScript::~GameOverLabelScript()
{
}

const std::string & GameOverLabelScript::getType() const
{
	return Type;
}

bool GameOverLabelScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	//Load the text in the xmlElement.
	auto labelDataElement = xmlElement->FirstChildElement("LabelData");
	if (labelDataElement)
		pimpl->m_GameOverText = labelDataElement->Attribute("Text");

	//Load the font data in the xmlElement.
	auto fontElement = xmlElement->FirstChildElement("Font");
	if (fontElement){
		pimpl->m_GameOverFontName = fontElement->Attribute("Name");
		pimpl->m_GameOverFontSize = fontElement->FloatAttribute("Size");
	}

	return true;
}

void GameOverLabelScript::vPostInit()
{
	//#TODO: Move the code for creating other components into xml.
	assert(!m_Actor.expired());
	auto strongActor = m_Actor.lock();

	pimpl->m_label_underlying = strongActor->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[this]{return cocos2d::Label::createWithSystemFont(pimpl->m_GameOverText, pimpl->m_GameOverFontName, pimpl->m_GameOverFontSize); });

	pimpl->m_label_underlying->setVisible(false);

	pimpl->m_invoker = strongActor->addComponent<SequentialInvoker>().get();

	pimpl->registerAsEventListeners();
}

const std::string GameOverLabelScript::Type = "GameOverLabelScript";
