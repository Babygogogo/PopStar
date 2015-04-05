#include "Audio.h"
#include "SimpleAudioEngine.h"

Audio* Audio::m_instance = nullptr;

Audio* Audio::getInstance(){
	if(m_instance == nullptr){
		m_instance = new Audio();
	}
	return m_instance;
}

void Audio::playBGM(){
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("Music/music.mp3", true);
}

void Audio::playReadyGo(){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Music/readygo.mp3",false);
}

void Audio::playPop(){
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Music/pop.mp3",false);
}

void Audio::playCombo(int size){
	if(size < 5)
		return;
	if(size >= 10){
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Music/combo_3.mp3",false);
	}else if(size >= 7){
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Music/combo_2.mp3",false);
	}else {
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Music/combo_1.mp3",false);
	}
}
void Audio::prepare(){
	auto audio_engine = CocosDenshion::SimpleAudioEngine::getInstance();
	audio_engine->preloadBackgroundMusic("Music/music.mp3");
	audio_engine->preloadEffect("Music/readygo.mp3");
	audio_engine->preloadEffect("Music/pop.mp3");
	audio_engine->preloadEffect("Music/combo_1.mp3");
	audio_engine->preloadEffect("Music/combo_2.mp3");
	audio_engine->preloadEffect("Music/combo_3.mp3");
}