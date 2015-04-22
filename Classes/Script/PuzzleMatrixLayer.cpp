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
#include "../Script/Stepper.h"
#include "../Event/Event.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"

struct PuzzleMatrixLayer::impl : public cocos2d::Layer
{
	impl(){};
	~impl();

	CREATE_FUNC(PuzzleMatrixLayer::impl);
	bool init();

	void floatLeftStarMsg(int leftNum);
	void gotoGameOver();

public:
	enum class State{
		BeforeLevelPreparation,
		LevelPreparationIdle,
		LevelInProgress
	};
	
	PuzzleMatrixLayer::impl *initializeLayer(GameObject *game_object);
	std::unique_ptr<GameObject> createBackground();
	std::unique_ptr<GameObject> createScoringLabel();
	std::unique_ptr<GameObject> createLevelPreparationLabel();

	void handleLayerTouch();

	State m_state{ State::BeforeLevelPreparation };
	GameObject *m_game_object{ nullptr };
	Stepper *XXX_preparation_label_stepper{ nullptr };
	
private:
	void showStarMatrix();

	LegacyStarMatrix* matrix;
};

bool PuzzleMatrixLayer::impl::init(){
	if (!Layer::init()){
		return false;
	}

	matrix = nullptr;
	this->scheduleUpdate();

	return true;
}

void PuzzleMatrixLayer::impl::showStarMatrix()
{
	if (matrix)
		this->removeChild(matrix);

	matrix = LegacyStarMatrix::create(
		[this](int num){this->floatLeftStarMsg(num); },
		[this](){this->gotoGameOver(); }
	);

	this->addChild(matrix);
}

void PuzzleMatrixLayer::impl::floatLeftStarMsg(int leftNum)
{
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	FloatWord* leftStarMsg1 = FloatWord::create(ChineseWord("shengyu") + cocos2d::String::createWithFormat("%d", leftNum)->_string + ChineseWord("ge"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 2));
	this->addChild(leftStarMsg1);

	int jiangLiScore = GAMEDATA::getInstance()->getEndLevelBonus(leftNum);
	FloatWord* leftStarMsg2 = FloatWord::create(ChineseWord("jiangli") + cocos2d::String::createWithFormat("%d", jiangLiScore)->_string + ChineseWord("fen"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 2 - 50));
	this->addChild(leftStarMsg2);

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
	this->addChild(gameOver);

	gameOver->floatIn(1.0f,
		[](){
		auto title_scene = GameObject::create();
		title_scene->addComponent<TitleScene>();
		SingletonContainer::instance()->get<SceneStack>()->replaceAndRun(std::move(title_scene));
	});
}

PuzzleMatrixLayer::impl * PuzzleMatrixLayer::impl::initializeLayer(GameObject *game_object)
{
	auto layer_underlying = game_object->addComponent<DisplayNode>()->initAs<PuzzleMatrixLayer::impl>();
	layer_underlying->m_game_object = game_object;

	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->setSwallowTouches(true);
	touch_listener->onTouchBegan = [layer_underlying](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		layer_underlying->handleLayerTouch();
		return true; };
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, layer_underlying);

	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelUp, layer_underlying,
		[layer_underlying](Event *){layer_underlying->m_state = State::BeforeLevelPreparation; });

	return layer_underlying;
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
	auto label_underlying = label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>(
		[]{return cocos2d::Label::createWithSystemFont(
			std::string("Level: ") + std::to_string(GAMEDATA::getInstance()->getCurrentLevel()) + '\n' + std::string(" Start!")
			, "Verdana-Bold", 50); });

	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width + label_underlying->getContentSize().width / 2, visible_size.height / 2);

	auto stepper = XXX_preparation_label_stepper = label_object->addComponent<Stepper>();
	stepper->addStep(0.6f, visible_size.width / 2, visible_size.height / 2);
	stepper->addStep(0.6f, -label_underlying->getContentSize().width / 2, visible_size.height / 2, [this]{showStarMatrix(); });
	
	Audio::getInstance()->playReadyGo();

	return label_object;
}

PuzzleMatrixLayer::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void PuzzleMatrixLayer::impl::handleLayerTouch()
{
	switch (m_state)
	{
	case State::BeforeLevelPreparation:
		m_game_object->addChild(createLevelPreparationLabel());
		if (XXX_preparation_label_stepper->nextStep())
			m_state = State::LevelPreparationIdle;
		break;
	case State::LevelPreparationIdle:
		if (XXX_preparation_label_stepper->nextStep())
			m_state = State::LevelInProgress;
		break;
	default:
		break;
	}
}

PuzzleMatrixLayer::PuzzleMatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl)
{
	auto layer = pimpl->initializeLayer(game_object);

	game_object->addChild(pimpl->createBackground());
	game_object->addChild(GameObject::create<StatusBar>());
	game_object->addChild(pimpl->createScoringLabel());

	layer->handleLayerTouch();
}

PuzzleMatrixLayer::~PuzzleMatrixLayer()
{
}
