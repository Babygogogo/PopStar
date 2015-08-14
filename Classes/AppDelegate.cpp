#include "AppDelegate.h"
#include "Engine/Audio/Audio.h"

#include "Engine/GameLogic/GameLogic.h"
#include "Engine/Utilities/SingletonContainer.h"
#include "Engine/MainLoop/Timer.h"
#include "Engine/Graphic2D/SceneStack.h"
#include "Common/GameData.h"
#include "Engine/Event/EventDispatcher.h"
#include "Engine/Script/TitleSceneScript.h"
#include "Engine/Actor/Actor.h"

USING_NS_CC;

//////////////////////////////////////////////////////////////////////////
//Definition of AppDelegate::AppDelegateImpl
//////////////////////////////////////////////////////////////////////////
struct AppDelegate::AppDelegateImpl
{
public:
	AppDelegateImpl();
	~AppDelegateImpl();

	void initGame();

	void update(float deltaTimeF);

	std::chrono::steady_clock::time_point lastUpdateTimePoint{ std::chrono::steady_clock::now() };
};

AppDelegate::AppDelegateImpl::AppDelegateImpl()
{
}

AppDelegate::AppDelegateImpl::~AppDelegateImpl()
{
	Director::getInstance()->getScheduler()->unscheduleUpdate(this);
}

void AppDelegate::AppDelegateImpl::initGame()
{
	Audio::getInstance()->prepare();

	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->set<::Timer>(::Timer::create())->init();
	singletonContainer->set<IEventDispatcher>(::EventDispatcher::create());
	singletonContainer->set<GameData>(GameData::create());
	singletonContainer->set<GameLogic>(std::make_unique<GameLogic>());
	singletonContainer->set<SceneStack>(std::make_unique<SceneStack>());

	//auto titleSceneActor = std::make_shared<Actor>();
	//titleSceneActor->addComponent<TitleSceneScript>();
	//singletonContainer->get<SceneStack>(std::make_unique<SceneStack>())->pushAndRun(std::move(titleSceneActor));
	auto titleScene = singletonContainer->get<GameLogic>()->createActor("Actors\\TitleScene.xml");
	singletonContainer->get<SceneStack>()->pushAndRun(std::move(titleScene));

	Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

void AppDelegate::AppDelegateImpl::update(float deltaTimeF)
{
	auto currentTimePoint = std::chrono::steady_clock::now();
	auto deltaTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTimePoint - lastUpdateTimePoint);
	lastUpdateTimePoint = std::move(currentTimePoint);

	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->get<IEventDispatcher>()->vDispatchQueuedEvents();
	singletonContainer->get<GameLogic>()->vUpdate(deltaTimeMs);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of AppDelegate
//////////////////////////////////////////////////////////////////////////
AppDelegate::AppDelegate() : pimpl{ std::make_unique<AppDelegateImpl>() }
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
		glview = GLView::create("PopStar");
		director->setOpenGLView(glview);
	}

	//set resolution
	glview->setFrameSize(480, 800);
	glview->setDesignResolutionSize(480, 800, ResolutionPolicy::SHOW_ALL);

	// turn on display FPS
	director->setDisplayStats(true);
	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	//Initialize the game.
	pimpl->initGame();

	return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
	Director::getInstance()->stopAnimation();

	// if you use SimpleAudioEngine, it must be pause
	// SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
	Director::getInstance()->startAnimation();

	// if you use SimpleAudioEngine, it must resume here
	// SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
