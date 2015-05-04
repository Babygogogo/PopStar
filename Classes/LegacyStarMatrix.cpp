#include "LegacyStarMatrix.h"
#include "StarParticle.h"
#include "ComboEffect.h"
#include "Audio.h"
#include <ctime>

#include "./Common/SingletonContainer.h"
#include "./Common/GameData.h"
#include "./Event/EventDispatcher.h"
#include "./Event/EventType.h"
#include "./Event/Event.h"
#include "./Event/EventArg1.h"

using namespace cocos2d;
using namespace std;

float LegacyStarMatrix::ONE_CLEAR_TIME = 0.05f;

LegacyStarMatrix * LegacyStarMatrix::create()
{
	auto matrix = new LegacyStarMatrix();
	if (matrix && matrix->init()){
		matrix->autorelease();
		return matrix;
	}

	CC_SAFE_DELETE(matrix);
	return nullptr;
}

bool LegacyStarMatrix::init()
{
	if (!Node::init())
		return false;

	needClear = false;
	clearSumTime = 0;
	memset(stars, 0, sizeof(LegacyStar*) * ROW_NUM * COL_NUM);
	initMatrix();

	registerTouchListener();
	this->scheduleUpdate();
	SingletonContainer::instance()->get<::EventDispatcher>()->registerListener(EventType::LevelSummaryLabelDisappeared, this, [this](::Event*){setNeedClear(true); });

	return true;
}

void LegacyStarMatrix::registerTouchListener()
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		onTouch(cocos2d::Director::getInstance()->convertToGL(touch->getLocationInView()));

		return true;
	};

	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void LegacyStarMatrix::update(float delta)
{
	for (int i = 0; i < ROW_NUM; i++){
		for (int j = 0; j < COL_NUM; j++){
			if (stars[i][j] != nullptr){
				stars[i][j]->updatePosition();
			}
		}
	}
	if (needClear){
		clearSumTime += delta;
		if (clearSumTime > ONE_CLEAR_TIME){
			clearMatrixOneByOne();
			clearSumTime = 0;
		}
	}
}

void LegacyStarMatrix::onTouch(const cocos2d::Point& p){
	LegacyStar* s = getStarByTouch(p);
	if(s){
	genSelectedList(s);
	CCLOG("SIZE = %d",selectedList.size());
	deleteSelectedList();
	}
}

void LegacyStarMatrix::setNeedClear(bool b){
	needClear = b;
}
void LegacyStarMatrix::initMatrix(){
	srand(time(0));
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			int color = abs(rand()%LegacyStar::COLOR_MAX_NUM);
			if(color < 0)
				CCLOG("color i=%d,j=%d");
			LegacyStar* star = LegacyStar::create(color);
			stars[i][j] = star;
			star->setPosition(getPositionByIndex(i, j) + cocos2d::Point(0, 100));
			star->setDesPosition(getPositionByIndex(i,j));
			star->setMatrixIndex(i,j);
			this->addChild(star);
		}
	}
}

Point LegacyStarMatrix::getPositionByIndex(int i,int j){
	float x = j * LegacyStar::STAR_WIDTH + LegacyStar::STAR_WIDTH/2;
	float y = (LegacyStarMatrix::COL_NUM - i)*LegacyStar::STAR_HEIGHT - LegacyStar::STAR_HEIGHT/2;
	return Point(x,y);
}

LegacyStar* LegacyStarMatrix::getStarByTouch(const Point& p){
	int k = p.y/LegacyStar::STAR_HEIGHT;//这里要用K转一下int 不然得不到正确结果
	int i = ROW_NUM - 1 - k;
	int j = p.x/LegacyStar::STAR_WIDTH;
	if(i >= 0 && i < ROW_NUM && 
	   j >= 0 && j < COL_NUM &&
	   stars[i][j] != nullptr){
		CCLOG("i=%d,j=%d",i,j);
		return stars[i][j];
	}else{
		return nullptr;
	}
}

void LegacyStarMatrix::genSelectedList(LegacyStar* s){
	selectedList.clear();
	deque<LegacyStar*> travelList;
	travelList.push_back(s);
	deque<LegacyStar*>::iterator it;
	for(it= travelList.begin();it != travelList.end();){
		LegacyStar* star = *it;
		LegacyStar* linkStar = nullptr;
		int index_i = star->getRowNum();
		int index_j = star->getColNum();
		//上
		if(index_i-1 >= 0 && (linkStar = stars[index_i-1][index_j]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i-1][index_j]);
		}
		//下
		if(index_i+1 < ROW_NUM  && (linkStar = stars[index_i+1][index_j]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i+1][index_j]);
		}
		//左
		if(index_j-1 >= 0 && (linkStar = stars[index_i][index_j-1]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j-1]);
		}
		//右
		if(index_j+1 < COL_NUM && (linkStar = stars[index_i][index_j+1]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j+1]);
		}
		if(!star->isSelected()){
			star->setSelected(true);
			selectedList.push_back(star);
		}
		travelList.pop_front();
		it = travelList.begin();
	}
}

void LegacyStarMatrix::deleteSelectedList(){
	if(selectedList.size() <= 1){
		selectedList.at(0)->setSelected(false);
		return;
	}

	for(auto it = selectedList.begin();it != selectedList.end();it++){
		LegacyStar* star = *it;
		//粒子效果
		showStarParticleEffect(star->getColor(),star->getPosition(),this);
		stars[star->getRowNum()][star->getColNum()] = nullptr;
		star->removeFromParentAndCleanup(true);
		//播放音效
		Audio::getInstance()->playPop();
	}
	//COMBO效果
	showComboEffect(selectedList.size(),this);
	Audio::getInstance()->playCombo(selectedList.size());

	refreshScore();
	adjustMatrix();

	if(isEnded()){
		//////////////////////////////////////////////////////////////////////////
		//m_layer->floatLeftStarMsg(getLeftStarNum());//通知layer弹出剩余星星的信息
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelNoMoreMove, EventArg1::create(getLeftStarNum())));
		//////////////////////////////////////////////////////////////////////////
		
		CCLOG("ENDED");
	}

}

void LegacyStarMatrix::adjustMatrix(){
	//垂直方向调整
	for(int i = ROW_NUM-1;i>=0;i--){
		for(int j = COL_NUM-1;j>=0;j--){
			if(stars[i][j] == nullptr){
				int up = i;
				int dis = 0;
				while(stars[up][j] == nullptr){
					dis++;
					up--;
					if(up<0){
						break;
					}
				}
				
				for(int begin_i = i - dis;begin_i >= 0;begin_i--){
					if(stars[begin_i][j] == nullptr)
						continue;
					LegacyStar* s = stars[begin_i + dis][j] = stars[begin_i][j];
					s->setMatrixIndex(begin_i + dis,j);
					s->setDesPosition(getPositionByIndex(begin_i + dis,j));
					stars[begin_i][j] = nullptr;
				}
			}else{
				continue;
			}
		}
	}
	//水平方向调整
	for(int j = 0;j < COL_NUM;j++){
		if(stars[ROW_NUM-1][j] == nullptr){
			int des = 0;
			int right = j;
			while(stars[ROW_NUM-1][right] == nullptr){
				des++;
				right++;
			}
			for(int begin_i = 0;begin_i<ROW_NUM;begin_i++){
				for(int begin_j = j + des;begin_j < COL_NUM;begin_j++){
					if(stars[begin_i][begin_j] == nullptr)
						continue;
					LegacyStar* s = stars[begin_i][begin_j - des] = stars[begin_i][begin_j];
					s->setMatrixIndex(begin_i,begin_j - des);
					s->setDesPosition(getPositionByIndex(begin_i,begin_j - des));
					stars[begin_i][begin_j] = nullptr;
				}
			}
		}
	}

}

void LegacyStarMatrix::refreshScore()
{
	SingletonContainer::instance()->get<GameData>()->updateCurrentScoreWith(selectedList.size());
}

bool LegacyStarMatrix::isEnded(){
	bool bRet = true;
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			int curColor = stars[i][j]->getColor();
			//上
			if(i-1>=0 && stars[i-1][j]!=nullptr && stars[i-1][j]->getColor() ==  curColor)
				return false;
			//下
			if(i+1<ROW_NUM && stars[i+1][j]!=nullptr && stars[i+1][j]->getColor() == curColor)
				return false;
			//左
			if(j-1>=0 && stars[i][j-1]!=nullptr && stars[i][j-1]->getColor() == curColor)
				return false;
			//右
			if(j+1<COL_NUM && stars[i][j+1]!=nullptr && stars[i][j+1]->getColor() == curColor)
				return false;
		}
	}
	return bRet;
}

void LegacyStarMatrix::clearMatrixOneByOne(){
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			showStarParticleEffect(stars[i][j]->getColor(),stars[i][j]->getPosition(),this);
			stars[i][j]->removeFromParentAndCleanup(true);
			stars[i][j] = nullptr;
			return;
		}
	}
	//能够执行到这一句说明Matrix全为空，不在需要清空
	needClear = false;
	
	//转到下一关或者弹出结束游戏的窗口
	//auto game_data = SingletonContainer::instance()->get<GameData>();
	//if(game_data->getCurrentScore() >= game_data->getTargetScore()){
	//	game_data->levelUp();
	//	
	//	//////////////////////////////////////////////////////////////////////////
	//	//m_layer->gotoNextLevel();
	//	//m_layerGotoNextLevel();
	//	SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelUp));
	//	//////////////////////////////////////////////////////////////////////////
	//}else{
	//	//////////////////////////////////////////////////////////////////////////
	//	//m_layer->gotoGameOver();
	//	//m_layerGotoGameOver();
	//	SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::GameOver));
	//	//////////////////////////////////////////////////////////////////////////
	//	CCLOG("GAME OVER");
	//}
	SingletonContainer::instance()->get<GameData>()->levelEnd();
}

int LegacyStarMatrix::getLeftStarNum(){
	int ret = 0;
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			ret++;
		}
	}
	return ret;
}

LegacyStarMatrix::~LegacyStarMatrix()
{
	auto singleton_container = SingletonContainer::instance();
	if (singleton_container)
		singleton_container->get<::EventDispatcher>()->deleteListener(this);
}
