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

	void gotoGameOver();

public:
	void addEventListeners();

	std::unique_ptr<GameObject> createBackground();
	std::unique_ptr<GameObject> createScoringLabel();

	void createAndAttachLevelMessageLabel(GameObject *layer_object, GameObject *&label_object);
	void resetLevelMessageLabel(GameObject *label_object, const std::string &text, std::function<void()> &&callback_after_disappear);
	void resetTouchListenerForLevelMessageLabel(GameObject *label_object);
	void resetStepperForLevelMessageLabel(GameObject *label_object, std::function<void()> &&callback_after_disappear);

	void startLevel();
	void showLevelSummary(int num_stars_left);

	cocos2d::Layer *m_layer_underlying{ nullptr };
	GameObject *m_preparation_label{ nullptr };
	GameObject *m_summary_label{ nullptr };
	
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

void PuzzleMatrixLayer::impl::addEventListeners()
{
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelUp, this,
		[this](Event *){startLevel(); });
	SingletonContainer::instance()->get<EventDispatcher>()->registerListener(EventType::LevelNoMoreMove, this,
		[this](Event *e){showLevelSummary(static_cast<EventArg1*>(e->getArg())->getInt()); });
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

void PuzzleMatrixLayer::impl::createAndAttachLevelMessageLabel(GameObject *layer_object, GameObject *&label_object)
{
	auto new_label_object = GameObject::create();
	new_label_object->addComponent<DisplayNode>()->initAs<cocos2d::Label>([]{return cocos2d::Label::createWithSystemFont("", "Verdana-Bold", 50); });
	new_label_object->addComponent<Stepper>();

	label_object = new_label_object.get();
	layer_object->addChild(std::move(new_label_object));
}

void PuzzleMatrixLayer::impl::resetLevelMessageLabel(GameObject *label_object, const std::string &text, std::function<void()> &&callback_after_disappear)
{
	auto label_underlying = label_object->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	label_underlying->setString(text);
	label_underlying->setVisible(true);

	resetStepperForLevelMessageLabel(label_object, std::move(callback_after_disappear));
	resetTouchListenerForLevelMessageLabel(label_object);
}

void PuzzleMatrixLayer::impl::resetStepperForLevelMessageLabel(GameObject *label_object, std::function<void()> &&callback_after_disappear)
{
	auto label_underlying = label_object->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	auto visible_size = cocos2d::Director::getInstance()->getVisibleSize();
	label_underlying->setPosition(visible_size.width + label_underlying->getContentSize().width / 2, visible_size.height / 2);

	auto stepper = label_object->getComponent<Stepper>();
	stepper->addStep(0.6f, visible_size.width / 2, visible_size.height / 2);
	stepper->addStep(0.6f, -label_underlying->getContentSize().width / 2, visible_size.height / 2, std::move(callback_after_disappear));
	stepper->nextStep();
}

void PuzzleMatrixLayer::impl::resetTouchListenerForLevelMessageLabel(GameObject *label_object)
{
	auto stepper = label_object->getComponent<Stepper>();
	auto touch_listener = cocos2d::EventListenerTouchOneByOne::create();
	touch_listener->onTouchBegan = [stepper](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		stepper->nextStep();
		return true; };

	auto label_underlying = label_object->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touch_listener, label_underlying);
}

PuzzleMatrixLayer::impl::~impl()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void PuzzleMatrixLayer::impl::startLevel()
{
	auto label_underlying = m_preparation_label->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	resetLevelMessageLabel(m_preparation_label, std::string("Level: ") + std::to_string(GAMEDATA::getInstance()->getCurrentLevel()) + '\n' + std::string(" Start!"),
		[this, label_underlying]{
			showStarMatrix();
			label_underlying->setVisible(false);
			cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(label_underlying); });

	Audio::getInstance()->playReadyGo();
}

void PuzzleMatrixLayer::impl::showLevelSummary(int num_stars_left)
{
	auto label_underlying = m_summary_label->getComponent<DisplayNode>()->getAs<cocos2d::Label>();
	resetLevelMessageLabel(m_preparation_label,
		std::string("Star Left: ") + std::to_string(num_stars_left) + '\n' + std::string("   Bonus: ") + std::to_string(GAMEDATA::getInstance()->getEndLevelBonus(num_stars_left)),
		[this, label_underlying, num_stars_left]{
			GAMEDATA::getInstance()->updateEndLevelScoreWith(num_stars_left);
			SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(Event::create(EventType::LevelResultEnded));
			label_underlying->setVisible(false);
			cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(label_underlying); });
}

PuzzleMatrixLayer::PuzzleMatrixLayer(GameObject *game_object) :Script("PuzzleMatrixLayer", game_object), pimpl(new impl)
{
	pimpl->m_layer_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	game_object->addChild(pimpl->createBackground());
	game_object->addChild(GameObject::create<StatusBar>());
	game_object->addChild(pimpl->createScoringLabel());

	pimpl->createAndAttachLevelMessageLabel(game_object, pimpl->m_preparation_label);
	pimpl->createAndAttachLevelMessageLabel(game_object, pimpl->m_summary_label);

	pimpl->addEventListeners();
	pimpl->startLevel();
}

PuzzleMatrixLayer::~PuzzleMatrixLayer()
{
}
