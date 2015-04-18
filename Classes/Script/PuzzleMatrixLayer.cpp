#include "PuzzleMatrixLayer.h"
#include "cocos2d.h"

#include "./Classes/FloatWord.h"
#include "./Classes/TopMenu.h"
#include "./Classes/StarMatrix.h"
#include "./Classes/Chinese.h"
#include "./Classes/GameData.h"
#include "./Classes/Audio.h"

#include "TitleScene.h"
#include "../Common/SingletonContainer.h"
#include "../Common/SceneStack.h"
#include "../GameObject/GameObject.h"
#include "../GameObject/DisplayNode.h"

struct PuzzleMatrixLayer::impl : public cocos2d::Layer
{
	impl(){};
	~impl(){};

	virtual bool init();
	CREATE_FUNC(PuzzleMatrixLayer::impl);
	void floatLevelWord();
	void floatTargetScoreWord();
	void removeFloatWord();
	void showStarMatrix();
	void update(float delta);
	bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
	void refreshMenu();
	void showLinkNum(int size);
	void hideLinkNum();
	void floatLeftStarMsg(int leftNum);
	void gotoNextLevel();
	void gotoGameOver();

	FloatWord* _levelMsg;
	FloatWord* _targetScore;
	TopMenu* menu;
	StarMatrix* matrix;
	cocos2d::Label* linkNum;
};

bool PuzzleMatrixLayer::impl::init(){
	if (!Layer::init()){
		return false;
	}

	matrix = nullptr;
	this->scheduleUpdate();

	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(PuzzleMatrixLayer::impl::onTouchBegan, this);
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	auto background = cocos2d::Sprite::create("bg_mainscene.jpg");
	background->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(background, -1);

	menu = TopMenu::create();
	this->addChild(menu);

	linkNum = cocos2d::Label::create("", "Arial", 40);
	linkNum->setPosition(visibleSize.width / 2, visibleSize.height - 250);
	linkNum->setVisible(false);
	this->addChild(linkNum, 1);

	this->floatLevelWord();

	return true;
}

void PuzzleMatrixLayer::impl::floatLevelWord(){
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	_levelMsg = FloatWord::create(
		ChineseWord("guanqia") + cocos2d::String::createWithFormat(": %d", GAMEDATA::getInstance()->getNextLevel())->_string,
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 3 * 2)
		);
	this->addChild(_levelMsg, 1);
	_levelMsg->floatIn(0.5f, CC_CALLBACK_0(PuzzleMatrixLayer::impl::floatTargetScoreWord, this));
	Audio::getInstance()->playReadyGo();
}

void PuzzleMatrixLayer::impl::floatTargetScoreWord(){
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	_targetScore = FloatWord::create(
		ChineseWord("mubiao") + cocos2d::String::createWithFormat(": %d", GAMEDATA::getInstance()->getNextScore())->_string + ChineseWord("fen"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 3)
		);
	this->addChild(_targetScore, 1);
	_targetScore->floatIn(0.5f, CC_CALLBACK_0(PuzzleMatrixLayer::impl::removeFloatWord, this));
}

void PuzzleMatrixLayer::impl::removeFloatWord(){
	_levelMsg->floatOut(0.5f, nullptr);
	_targetScore->floatOut(0.5f, CC_CALLBACK_0(PuzzleMatrixLayer::impl::showStarMatrix, this));
}

void PuzzleMatrixLayer::impl::showStarMatrix(){
	matrix = StarMatrix::create(
		[this](){this->hideLinkNum(); },
		[this](int num){this->showLinkNum(num); },
		[this](int num){this->floatLeftStarMsg(num); },
		[this](){this->refreshMenu(); },
		[this](){this->gotoNextLevel(); },
		[this](){this->gotoGameOver(); }
	);

	this->addChild(matrix);
}

void PuzzleMatrixLayer::impl::update(float delta){
	if (matrix){
		matrix->updateStar(delta);
	}
}

bool PuzzleMatrixLayer::impl::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event){
	auto p = touch->getLocationInView();
	p = cocos2d::Director::getInstance()->convertToGL(p);
	CCLOG("x=%f y=%f", p.x, p.y);
	if (matrix){
		matrix->onTouch(p);
	}
	return true;
}

void PuzzleMatrixLayer::impl::refreshMenu(){
	menu->refresh();
}

void PuzzleMatrixLayer::impl::showLinkNum(int size)
{
	auto s = cocos2d::String::createWithFormat("%d", size)->_string + ChineseWord("lianji") +
		cocos2d::String::createWithFormat("%d", size*size * 5)->_string + ChineseWord("fen");
	linkNum->setString(s);
	linkNum->setVisible(true);
}

void PuzzleMatrixLayer::impl::hideLinkNum(){
	linkNum->setVisible(false);
}

void PuzzleMatrixLayer::impl::floatLeftStarMsg(int leftNum){
	auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	FloatWord* leftStarMsg1 = FloatWord::create(ChineseWord("shengyu") + cocos2d::String::createWithFormat("%d", leftNum)->_string + ChineseWord("ge"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 2));
	this->addChild(leftStarMsg1);
	int jiangLiScore = GAMEDATA::getInstance()->getJiangli(leftNum);
	FloatWord* leftStarMsg2 = FloatWord::create(ChineseWord("jiangli") + cocos2d::String::createWithFormat("%d", jiangLiScore)->_string + ChineseWord("fen"),
		50, cocos2d::Point(visibleSize.width, visibleSize.height / 2 - 50));
	this->addChild(leftStarMsg2);

	leftStarMsg1->floatInOut(0.5f, 1.0f,
		[=](){
		hideLinkNum();
		matrix->setNeedClear(true);
		GAMEDATA* data = GAMEDATA::getInstance();
		data->setCurScore(data->getCurScore() + jiangLiScore);
		if (data->getCurScore() > data->getHistoryScore()){
			data->setHistoryScore(data->getCurScore());
		}
		refreshMenu();
	});
	leftStarMsg2->floatInOut(0.5f, 1.0f, nullptr);
}

void PuzzleMatrixLayer::impl::gotoNextLevel(){
	refreshMenu();
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
		SingletonContainer::instance().get<SceneStack>()->replaceAndRun(std::move(title_scene));
	});
}


PuzzleMatrixLayer::PuzzleMatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl)
{
	game_object->addComponent<DisplayNode>()->initAs<PuzzleMatrixLayer::impl>();
}

PuzzleMatrixLayer::~PuzzleMatrixLayer()
{

}

void PuzzleMatrixLayer::update(const std::time_t& time_ms)
{
	return;
}
