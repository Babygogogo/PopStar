#include "AppDelegate.h"
#include "Engine/Audio/Audio.h"

#include "Engine/Actor/Actor.h"
#include "Engine/Actor/BaseRenderComponent.h"
#include "Engine/Event/EventDispatcher.h"
#include "Engine/GameLogic/GameLogic.h"
#include "Engine/Graphic2D/SceneStack.h"
#include "Engine/MainLoop/Timer.h"
#include "Engine/Utilities/SingletonContainer.h"

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

	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Texture.plist");

	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->set<::Timer>(::Timer::create())->init();
	singletonContainer->set<IEventDispatcher>(std::make_unique<::EventDispatcher>());
	singletonContainer->set<GameLogic>(std::make_unique<GameLogic>());
	singletonContainer->set<SceneStack>(std::make_unique<SceneStack>());

	auto titleScene = singletonContainer->get<GameLogic>()->createActor("Actors\\TitleScene.xml");
	singletonContainer->get<SceneStack>()->pushAndRun(*titleScene);

	Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);

	//Test the particle explosion.
	auto pe = cocos2d::ParticleExplosion::create();
	pe->setTexture(cocos2d::Director::getInstance()->getTextureCache()->addImage("red_heart.png"));
	pe->setTotalParticles(10);
	pe->setPosition(240, 400);
	titleScene->getRenderComponent()->getSceneNode()->addChild(pe, 10);
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
static cocos2d::Size designResolutionSize = cocos2d::Size(480, 800);
static cocos2d::Size smallResolutionSize = cocos2d::Size(320, 480);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(768, 1024);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1536, 2048);

AppDelegate::AppDelegate() : pimpl{ std::make_unique<AppDelegateImpl>() }
{
}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
	//set OpenGL context attributions,now can only set six attributions:
	//red,green,blue,alpha,depth,stencil
	GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };

	GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
	return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
		glview = GLViewImpl::createWithRect("PopStar", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
		glview = GLViewImpl::create("PopStar");
#endif
		director->setOpenGLView(glview);
	}

	// turn on display FPS
	director->setDisplayStats(true);

	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	// Set the design resolution
	glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);

	//Adjust the content scale factor. Seems to be useless by now (useful when run on fixed-resolution devices).
	//Size frameSize = glview->getFrameSize();
	//// if the frame's height is larger than the height of medium size.
	//if (frameSize.height > mediumResolutionSize.height)
	//{
	//	director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
	//}
	//// if the frame's height is larger than the height of small size.
	//else if (frameSize.height > smallResolutionSize.height)
	//{
	//	director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
	//}
	//// if the frame's height is smaller than the height of medium size.
	//else
	//{
	//	director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
	//}

	register_all_packages();

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
