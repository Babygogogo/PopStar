#include "LegacyStarMatrix.h"
#include "GameData.h"
#include "StarParticle.h"
#include "ComboEffect.h"
#include "Audio.h"
#include <ctime>

#include "./Common/SingletonContainer.h"
#include "./Event/EventDispatcher.h"

using namespace cocos2d;
using namespace std;

float LegacyStarMatrix::ONE_CLEAR_TIME = 0.05f;

LegacyStarMatrix * LegacyStarMatrix::create(std::function<void()> &&layerHideLinkNum, std::function<void(int)> &&layerShowLinkNum,
	std::function<void(int)> &&layerFloatLeftStarMsg, std::function<void()> &&layerRefreshMenu,
	std::function<void()> &&layerGotoNextLevel, std::function<void()> &&layerGotoGameOver)
{
	auto matrix = new LegacyStarMatrix();
	if (matrix && matrix->init(std::move(layerHideLinkNum), std::move(layerShowLinkNum), std::move(layerFloatLeftStarMsg),
		std::move(layerRefreshMenu), std::move(layerGotoNextLevel), std::move(layerGotoGameOver))){
		matrix->autorelease();
		return matrix;
	}

	CC_SAFE_DELETE(matrix);
	return nullptr;
}

bool LegacyStarMatrix::init(std::function<void()> &&layerHideLinkNum, std::function<void(int)> &&layerShowLinkNum,
	std::function<void(int)> &&layerFloatLeftStarMsg, std::function<void()> &&layerRefreshMenu,
	std::function<void()> &&layerGotoNextLevel, std::function<void()> &&layerGotoGameOver)
{
	if (!Node::init())
		return false;

	m_layerHideLinkNum = std::move(layerHideLinkNum);
	m_layerShowLinkNum = std::move(layerShowLinkNum);
	m_layerFloatLeftStarMsg = std::move(layerFloatLeftStarMsg);
	m_layerRefreshMenu = std::move(layerRefreshMenu);
	m_layerGotoNextLevel = std::move(layerGotoNextLevel);
	m_layerGotoGameOver = std::move(layerGotoGameOver);

	needClear = false;
	clearSumTime = 0;
	memset(stars, 0, sizeof(Star*) * ROW_NUM * COL_NUM);
	initMatrix();

	registerTouchListener();
	this->scheduleUpdate();
	SingletonContainer::instance()->get<::EventDispatcher>()->registerListener(EventType::LevelResultPanelClosed, this, [this]{setNeedClear(true); });

	return true;
}

void LegacyStarMatrix::registerTouchListener()
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

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
	Star* s = getStarByTouch(p);
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
			int color = abs(rand()%Star::COLOR_MAX_NUM);
			if(color < 0)
				CCLOG("color i=%d,j=%d");
			Star* star = Star::create(color);
			stars[i][j] = star;
			star->setPosition(getPositionByIndex(i, j) + cocos2d::Point(0, 100));
			star->setDesPosition(getPositionByIndex(i,j));
			star->setIndex_ij(i,j);
			this->addChild(star);
		}
	}
}

Point LegacyStarMatrix::getPositionByIndex(int i,int j){
	float x = j * Star::STAR_WIDTH + Star::STAR_WIDTH/2;
	float y = (LegacyStarMatrix::COL_NUM - i)*Star::STAR_HEIGHT - Star::STAR_HEIGHT/2;
	return Point(x,y);
}

Star* LegacyStarMatrix::getStarByTouch(const Point& p){
	int k = p.y/Star::STAR_HEIGHT;//����Ҫ��Kתһ��int ��Ȼ�ò�����ȷ���
	int i = ROW_NUM - 1 - k;
	int j = p.x/Star::STAR_WIDTH;
	if(i >= 0 && i < ROW_NUM && 
	   j >= 0 && j < COL_NUM &&
	   stars[i][j] != nullptr){
		CCLOG("i=%d,j=%d",i,j);
		return stars[i][j];
	}else{
		return nullptr;
	}
}

void LegacyStarMatrix::genSelectedList(Star* s){
	selectedList.clear();
	deque<Star*> travelList;
	travelList.push_back(s);
	deque<Star*>::iterator it;
	for(it= travelList.begin();it != travelList.end();){
		Star* star = *it;
		Star* linkStar = nullptr;
		int index_i = star->getIndexI();
		int index_j = star->getIndexJ();
		//��
		if(index_i-1 >= 0 && (linkStar = stars[index_i-1][index_j]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i-1][index_j]);
		}
		//��
		if(index_i+1 < ROW_NUM  && (linkStar = stars[index_i+1][index_j]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i+1][index_j]);
		}
		//��
		if(index_j-1 >= 0 && (linkStar = stars[index_i][index_j-1]) ){
			if(!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j-1]);
		}
		//��
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

		//////////////////////////////////////////////////////////////////////////
		//m_layer->hideLinkNum();
		m_layerHideLinkNum();
		//////////////////////////////////////////////////////////////////////////
		
		selectedList.at(0)->setSelected(false);
		return;
	}

	for(auto it = selectedList.begin();it != selectedList.end();it++){
		Star* star = *it;
		//����Ч��
		showStarParticleEffect(star->getColor(),star->getPosition(),this);
		stars[star->getIndexI()][star->getIndexJ()] = nullptr;
		star->removeFromParentAndCleanup(true);
		//������Ч
		Audio::getInstance()->playPop();
	}
	//COMBOЧ��
	showComboEffect(selectedList.size(),this);
	Audio::getInstance()->playCombo(selectedList.size());

	refreshScore();
	
	//////////////////////////////////////////////////////////////////////////
	//m_layer->showLinkNum(selectedList.size());
	m_layerShowLinkNum(selectedList.size());
	//////////////////////////////////////////////////////////////////////////

	adjustMatrix();
	if(isEnded()){
		//////////////////////////////////////////////////////////////////////////
		//m_layer->floatLeftStarMsg(getLeftStarNum());//֪ͨlayer����ʣ�����ǵ���Ϣ
		m_layerFloatLeftStarMsg(getLeftStarNum());
		//////////////////////////////////////////////////////////////////////////
		
		CCLOG("ENDED");
	}

}

void LegacyStarMatrix::adjustMatrix(){
	//��ֱ�������
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
					Star* s = stars[begin_i + dis][j] = stars[begin_i][j];
					s->setIndex_ij(begin_i + dis,j);
					s->setDesPosition(getPositionByIndex(begin_i + dis,j));
					stars[begin_i][j] = nullptr;
				}
			}else{
				continue;
			}
		}
	}
	//ˮƽ�������
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
					Star* s = stars[begin_i][begin_j - des] = stars[begin_i][begin_j];
					s->setIndex_ij(begin_i,begin_j - des);
					s->setDesPosition(getPositionByIndex(begin_i,begin_j - des));
					stars[begin_i][begin_j] = nullptr;
				}
			}
		}
	}

}


void LegacyStarMatrix::refreshScore(){
	GAMEDATA* data = GAMEDATA::getInstance();
	data->setCurScore(data->getCurScore() + selectedList.size()*selectedList.size()*5);
	if(data->getCurScore() > data->getHistoryScore()){
		data->setHistoryScore(data->getCurScore());
	}

	//////////////////////////////////////////////////////////////////////////
	//m_layer->refreshMenu();
	m_layerRefreshMenu();
	//////////////////////////////////////////////////////////////////////////
}


bool LegacyStarMatrix::isEnded(){
	bool bRet = true;
	for(int i=0;i<ROW_NUM;i++){
		for(int j=0;j<COL_NUM;j++){
			if(stars[i][j] == nullptr)
				continue;
			int curColor = stars[i][j]->getColor();
			//��
			if(i-1>=0 && stars[i-1][j]!=nullptr && stars[i-1][j]->getColor() ==  curColor)
				return false;
			//��
			if(i+1<ROW_NUM && stars[i+1][j]!=nullptr && stars[i+1][j]->getColor() == curColor)
				return false;
			//��
			if(j-1>=0 && stars[i][j-1]!=nullptr && stars[i][j-1]->getColor() == curColor)
				return false;
			//��
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
	//�ܹ�ִ�е���һ��˵��MatrixȫΪ�գ�������Ҫ���
	needClear = false;
	//ת����һ�ػ��ߵ���������Ϸ�Ĵ���
	if(GAMEDATA::getInstance()->getCurScore() >= GAMEDATA::getInstance()->getNextScore()){
		GAMEDATA::getInstance()->setCurLevel(GAMEDATA::getInstance()->getCurLevel() + 1);
		
		//////////////////////////////////////////////////////////////////////////
		//m_layer->gotoNextLevel();
		m_layerGotoNextLevel();
		//////////////////////////////////////////////////////////////////////////
	}else{
		//////////////////////////////////////////////////////////////////////////
		//m_layer->gotoGameOver();
		m_layerGotoGameOver();
		//////////////////////////////////////////////////////////////////////////
		CCLOG("GAME OVER");
	}
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
