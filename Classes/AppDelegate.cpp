#include "AppDelegate.h"
#include "Audio.h"

#include "./Common/SingletonContainer.h"
#include "./Common/Timer.h"
#include "./Common/SceneStack.h"
#include "./Script/TitleScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
	// initialize director
	auto director = Director::getInstance();
	auto glview = director->getOpenGLView();
	if(!glview) {
		glview = GLView::create("POPSTAR");
		director->setOpenGLView(glview);
	}

	//set resolution
	glview->setFrameSize(480, 800);
	glview->setDesignResolutionSize(480, 800, ResolutionPolicy::SHOW_ALL);

	// turn on display FPS
	director->setDisplayStats(true);
	// set FPS. the default value is 1.0/60 if you don't call this
	director->setAnimationInterval(1.0 / 60);

	// create a scene and run
	Audio::getInstance()->prepare();

	auto title_scene = GameObject::create();
	title_scene->addComponent<TitleScene>();
	SingletonContainer::instance().add<::Timer>();
	SingletonContainer::instance().add<SceneStack>()->pushAndRun(std::move(title_scene));

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
