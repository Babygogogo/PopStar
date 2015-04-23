#include "PuzzleMatrixLayer.h"
#include "cocos2d.h"

#include "./Classes/FloatWord.h"
#include "./Classes/LegacyStarMatrix.h"
#include "./Classes/Chinese.h"
#include "./Classes/GameData.h"
#include "./Classes/Audio.h"

#include "TitleScene.h"
#include "StatusBar.h"
#include "../Common/SingletonContainer.h"
#include "../Common/SceneStack.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/Stepper.h"
#include "../Event/Event.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventArg1.h"

struct PuzzleMatrixLayer::impl
{
	impl(){};
	~impl();

	void floatLeftStarMsg(int leftNum);
	void gotoGameOver();

public:
	void initializeLayer(GameObject *game_object);

	std::unique_ptr<GameObject> createBackground();
	std::unique_ptr<GameObject> createScoringLabel();

	std::unique_ptr<GameObject> createLevelPreparationLabel();
	void resetLevelPreparationLabel();
	void resetTouchListenerForPreparationLabel();
	void resetStepperForPreparationLabel();

	void startLevel();

	cocos2d::Layer *m_layer_underlying{ nullptr };
	GameObject *m_preparation_label{ nullptr };
	
private:
	void showStarMatrix();

	LegacyStarMatrix* matrix{nullptr};
};

void PuzzleMatrixLayer::impl::showStarMatrix()
{
	if (matrix)
		m_layer_underlying->removeChild(matrix);

	matrix = LegacyStarMatrix::create();
	m_layer_underlying->addChild(matrix);
}

void PuzzleMatrixLayer::impl::floatLeftStarMsg(int leftNum)
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	FloatWord* leftStarMsg1 = FloatWord::create(ChineseWord("shengyu") + cocos2d::String::createWithFormat("%d", leftNum)->_string + ChineseWord("ge"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 2));
	m_layer_underlying->addChild(leftStarMsg1);

	int jiangLiScore = GAMEDATA::getInstance()->getEndLevelBonus(leftNum);
	FloatWord* leftStarMsg2 = FloatWord::create(ChineseWord("jiangli") + cocos2d::String::createWithFormat("%d", jiangLiScore)->_string + ChineseWord("fen"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 2 - 50));
	m_layer_underlying->addChild(leftStarMsg2);

	leftStarMsg1->floatInOut(0.5f, 1.0f,
		[=](){
		GAMEDATA::getInstance()->updateEndLevelScoreWith(leftNum);
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(Event::create(EventType::LevelResultEnded));
	});

	leftStarMsg2->floatInOut(0.5f, 1.0f, nullptr);
}

void PuzzleMatrixLayer::impl::gotoGameOver(){
	//Ʈ�֣��л�scene
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	FloatWord* gameOver = FloatWord::create(
		"GAME OVER", 80, cocos2d::Point(visibleSize.width, visibleSize.height / 2));
	//this->addChild(gameOver);
	m_layer_underlying->addChild(gameOver);

	gameOver->floatIn(1.0f,
		[](){
		auto title_scene = GameObject::create();
		title_scene->addComponent<TitleScene>();
		SingletonContainer::instance()->get<SceneStack>()->replaceAndRun(std::move(title_scene));
	});
}

void PuzzleMatrixLayer::impl::initializeLayer(GameObject *game_object)
{
	m_layer_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelUp, this,
		[this](Event *){startLevel(); });
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelNoMoreMove, this,
		[this](Event *e){floatLeftStarMsg(static_cast<EventArg1*>(e->getArg())->getInt()); });
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::GameOver, this,
		[this](Event *){gotoGameOver(); });
}

std::unique_ptr<GameObject> PuzzleMatrixLayer::impl::createBackground()
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	
	auto background_object = GameObject::create();
	auto background_sprite = background_object->addComponent<DisplayNode>()->
		initAs<cocos2d::Sprite>([](){return cocos2d::Sprite::create("bg_mainscene.jpg"); });

	background_sprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	background_sprite->setLocalZOrder(-1);

	return background_object;
}

std::unique_ptr<GameObject> PuzzleMatrixLayer::impl::createScoringLabel()
{
	auto label_object = GameObject::create();
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont("", "Arial", 30); });

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visibleSize.width / 2, visibleSize.height - 250);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::StarsExploded, this, [label_underlying](Event *){
		auto exploded_stars = std::to_string(GAMEDATA::getInstance()->getNumExplodedStars());
		auto attained_score = std::to_string(GAMEDATA::getInstance()->getScoreOfPreviousExplosion());
		label_underlying->setString(std::string("Exploded: ") + exploded_stars + std::string(" Score: ") + attained_score);

		label_underlying->setVisible(true); });

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelResultEnded, this, [label_underlying](Event*){
		label_underlying->setVisible(false); });

	return label_object;
}

std::unique_ptr<GameObject> PuzzleMatrixLayer::impl::createLevelPreparationLabel()
{
	auto label_object = GameObject::create();

	label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont("", "Verdana-Bold", 50); });
	label_object->addComponent<Stepper>();

	return label_object;
}

void PuzzleMatrixLayer::impl::resetLevelPreparationLabel()
{
	auto label_underlying = m_preparation_label->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	label_underlying->setString(std::string("Level: ") + std::to_string(GAMEDATA::getInstance()->getCurrentLevel()) + '\n' + std::string(" Start!"));
	label_underlying->setVisible(true);

	resetStepperForPreparationLabel();
	resetTouchListenerForPreparationLabel();
}

void PuzzleMatrixLayer::impl::resetTouchListenerForPreparationLabel()
{
	auto stepper = m_preparation_label->getComponent<Stepper>();
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [stepper](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		stepper->nextStep();
		return true; };

	auto label_underlying = m_preparation_label->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, label_underlying);
}

void PuzzleMatrixLayer::impl::resetStepperForPreparationLabel()
{
	auto label_underlying = m_preparation_label->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width + label_underlying->getContentSize().width / 2, visible_size.height / 2);

	auto stepper = m_preparation_label->getComponent<Stepper>();
	stepper->addStep(0.6f, visible_size.width / 2, visible_size.height / 2);
	stepper->addStep(0.6f, -label_underlying->getContentSize().width / 2, visible_size.height / 2, [this, label_underlying]{
		showStarMatrix();
		label_underlying->setVisible(false);
		cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(label_underlying); });
		stepper->nextStep();
}

PuzzleMatrixLayer::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void PuzzleMatrixLayer::impl::startLevel()
{
	resetLevelPreparationLabel();

	Audio::getInstance()->playReadyGo();
}

PuzzleMatrixLayer::PuzzleMatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl)
{
	pimpl->initializeLayer(game_object);

	game_object->addChild(pimpl->createBackground());
	game_object->addChild(GameObject::create<StatusBar>());
	game_object->addChild(pimpl->createScoringLabel());

	auto preparation_label = pimpl->createLevelPreparationLabel();
	pimpl->m_preparation_label = preparation_label.get();
	game_object->addChild(std::move(preparation_label));

	pimpl->startLevel();
}

PuzzleMatrixLayer::~PuzzleMatrixLayer()
{
}
