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
#include "../Event/Event.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"

struct PuzzleMatrixLayer::impl : public cocos2d::Layer
{
	impl(){};
	~impl();

	CREATE_FUNC(PuzzleMatrixLayer::impl);

	void floatLeftStarMsg(int leftNum);
	void gotoNextLevel();
	void gotoGameOver();

public:
	std::unique_ptr<GameObject> createBackground();
	std::unique_ptr<GameObject> createComboLabel();
	
private:
	bool init();

	void floatLevelWord();
	void floatTargetScoreWord();
	void removeFloatWord();
	void showStarMatrix();

	FloatWord* _levelMsg;
	FloatWord* _targetScore;
	LegacyStarMatrix* matrix;
};

bool PuzzleMatrixLayer::impl::init(){
	if (!Layer::init()){
		return false;
	}

	matrix = nullptr;
	this->scheduleUpdate();

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

	this->floatLevelWord();

	return true;
}

void PuzzleMatrixLayer::impl::floatLevelWord(){
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	_levelMsg = FloatWord::create(
		ChineseWord("guanqia") + cocos2d::String::createWithFormat(": %d", GAMEDATA::getInstance()->getCurrentLevel())->_string,
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 3 * 2)
		);
	this->addChild(_levelMsg, 1);
	_levelMsg->floatIn(0.5f, CC_CALLBACK_0(PuzzleMatrixLayer::impl::floatTargetScoreWord, this));
	Audio::getInstance()->playReadyGo();
}

void PuzzleMatrixLayer::impl::floatTargetScoreWord(){
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	_targetScore = FloatWord::create(
		ChineseWord("mubiao") + cocos2d::String::createWithFormat(": %d", GAMEDATA::getInstance()->getTargetScore())->_string + ChineseWord("fen"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 3)
		);
	this->addChild(_targetScore, 1);
	_targetScore->floatIn(0.5f, CC_CALLBACK_0(PuzzleMatrixLayer::impl::removeFloatWord, this));
}

void PuzzleMatrixLayer::impl::removeFloatWord(){
	_levelMsg->floatOut(0.5f, nullptr);
	_targetScore->floatOut(0.5f, CC_CALLBACK_0(PuzzleMatrixLayer::impl::showStarMatrix, this));
}

void PuzzleMatrixLayer::impl::showStarMatrix()
{
	if (matrix)
		this->removeChild(matrix);

	matrix = LegacyStarMatrix::create(
		[this](int num){this->floatLeftStarMsg(num); },
		[this](){this->gotoNextLevel(); },
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

void PuzzleMatrixLayer::impl::gotoNextLevel(){
	floatLevelWord();
}

void PuzzleMatrixLayer::impl::gotoGameOver(){
	//������߷�?
	GAMEDATA::getInstance()->saveHighestScore();
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

std::unique_ptr<GameObject> PuzzleMatrixLayer::impl::createComboLabel()
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

PuzzleMatrixLayer::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

PuzzleMatrixLayer::PuzzleMatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl)
{
	auto layer_underlying = game_object->addComponent<DisplayNode>()->initAs<PuzzleMatrixLayer::impl>();
	game_object->addChild(pimpl->createBackground());
	game_object->addChild(GameObject::create<StatusBar>());
	game_object->addChild(pimpl->createComboLabel());
}

PuzzleMatrixLayer::~PuzzleMatrixLayer()
{
}
