#include "AppDelegate.h"
#include "Engine/Audio/Audio.h"

#include "Engine/GameLogic/GameLogic.h"
#include "Engine/Utilities/SingletonContainer.h"
#include "Engine/MainLoop/Timer.h"
#include "Engine/Graphic2D/SceneStack.h"
#include "Common/GameData.h"
#include "Engine/Event/EventDispatcher.h"
#include "Engine/Script/TitleScene.h"
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
};

AppDelegate::AppDelegateImpl::AppDelegateImpl()
{

}

AppDelegate::AppDelegateImpl::~AppDelegateImpl()
{

}

void AppDelegate::AppDelegateImpl::initGame()
{
	Audio::getInstance()->prepare();

	SingletonContainer::getInstance()->set<::Timer>(::Timer::create())->init();
	SingletonContainer::getInstance()->set<IEventDispatcher>(::EventDispatcher::create());
	SingletonContainer::getInstance()->set<GameData>(GameData::create());
	SingletonContainer::getInstance()->set<GameLogic>(GameLogic::create());

	auto titleSceneActor = std::make_shared<Actor>();
	titleSceneActor->addComponent<TitleScene>();

	SingletonContainer::getInstance()->set<SceneStack>(std::make_unique<SceneStack>())->pushAndRun(std::move(titleSceneActor));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of AppDelegate
//////////////////////////////////////////////////////////////////////////
AppDelegate::AppDelegate() : pimpl{std::make_unique<AppDelegateImpl>()}
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
	if(!glview) {
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
