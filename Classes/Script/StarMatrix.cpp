#include "StarMatrix.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/GameObject.h"
#include "../Common/SingletonContainer.h"
#include "../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/Event.h"
#include "../Event/EventArg1.h"

//#include "LegacyStarMatrix.h"
#include "../Classes/StarParticle.h"
#include "../Classes/ComboEffect.h"
#include "../Classes/Audio.h"
#include "../Classes/Star.h"

#include <deque>

struct StarMatrix::impl
{
	impl(){};
	~impl(){};

public:
	void onTouch(const cocos2d::Point& p);
	void setNeedClear(bool b);
	void initMatrix();

	cocos2d::Point getPositionByIndex(int i, int j);
	Star* getStarByTouch(const cocos2d::Point& p);
	void genSelectedList(Star* s);
	void deleteSelectedList();
	void adjustMatrix();
	void refreshScore();
	bool isEnded();
	void clearMatrixOneByOne();
	int getLeftStarNum();

	void init();
	void registerTouchListener();
	void update(float delta);

public:
	const static int ROW_NUM = 10;
	const static int COL_NUM = 10;
	const static float ONE_CLEAR_TIME;

	cocos2d::Node *m_node_underlying{ nullptr };
	Star* stars[ROW_NUM][COL_NUM];
	std::deque<Star*> selectedList;
	bool needClear{ false };
	float clearSumTime{ 0 };
};

const float StarMatrix::impl::ONE_CLEAR_TIME = 0.05f;

void StarMatrix::impl::init()
{
	initMatrix();
	registerTouchListener();
	SingletonContainer::instance()->get<::EventDispatcher>()->registerListener(EventType::LevelResultEnded, this, [this](::Event*){setNeedClear(true); });
}

void StarMatrix::impl::initMatrix()
{
	srand(time(0));
	for (int i = 0; i < ROW_NUM; i++){
		for (int j = 0; j < COL_NUM; j++){
			int color = abs(rand() % Star::COLOR_MAX_NUM);
			Star* star = Star::create(color);
			stars[i][j] = star;
			star->setPosition(getPositionByIndex(i, j) + cocos2d::Point(0, 100));
			star->setDesPosition(getPositionByIndex(i, j));
			star->setIndex_ij(i, j);
			m_node_underlying->addChild(star);
		}
	}
}

void StarMatrix::impl::setNeedClear(bool b)
{
	needClear = b;
}

void StarMatrix::impl::registerTouchListener()
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		onTouch(cocos2d::Director::getInstance()->convertToGL(touch->getLocationInView()));
		return true;
	};

	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_node_underlying);
}

void StarMatrix::impl::onTouch(const cocos2d::Point& p)
{
	if (auto star = getStarByTouch(p)){
		genSelectedList(star);
		deleteSelectedList();
	}
}

Star* StarMatrix::impl::getStarByTouch(const cocos2d::Point& p)
{
	int k = p.y / Star::STAR_HEIGHT;//这里要用K转一下int 不然得不到正确结果
	int i = ROW_NUM - 1 - k;
	int j = p.x / Star::STAR_WIDTH;

	if (i >= 0 && i < ROW_NUM &&
		j >= 0 && j < COL_NUM &&
		stars[i][j] != nullptr){
		CCLOG("i=%d,j=%d", i, j);
		return stars[i][j];
	}
	else{
		return nullptr;
	}
}

void StarMatrix::impl::genSelectedList(Star* s)
{
	selectedList.clear();
	std::deque<Star*> travelList;
	travelList.push_back(s);
	std::deque<Star*>::iterator it;
	for (it = travelList.begin(); it != travelList.end();){
		Star* star = *it;
		Star* linkStar = nullptr;
		int index_i = star->getIndexI();
		int index_j = star->getIndexJ();
		//上
		if (index_i - 1 >= 0 && (linkStar = stars[index_i - 1][index_j])){
			if (!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i - 1][index_j]);
		}
		//下
		if (index_i + 1 < ROW_NUM && (linkStar = stars[index_i + 1][index_j])){
			if (!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i + 1][index_j]);
		}
		//左
		if (index_j - 1 >= 0 && (linkStar = stars[index_i][index_j - 1])){
			if (!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j - 1]);
		}
		//右
		if (index_j + 1 < COL_NUM && (linkStar = stars[index_i][index_j + 1])){
			if (!linkStar->isSelected() && linkStar->getColor() == star->getColor())
				travelList.push_back(stars[index_i][index_j + 1]);
		}
		if (!star->isSelected()){
			star->setSelected(true);
			selectedList.push_back(star);
		}
		travelList.pop_front();
		it = travelList.begin();
	}
}

void StarMatrix::impl::deleteSelectedList()
{
	if (selectedList.size() <= 1){
		selectedList.at(0)->setSelected(false);
		return;
	}

	for (auto it = selectedList.begin(); it != selectedList.end(); it++){
		Star* star = *it;
		//粒子效果
		showStarParticleEffect(star->getColor(), star->getPosition(), m_node_underlying);
		stars[star->getIndexI()][star->getIndexJ()] = nullptr;
		star->removeFromParentAndCleanup(true);
		//播放音效
		Audio::getInstance()->playPop();
	}
	//COMBO效果
	showComboEffect(selectedList.size(), m_node_underlying);
	Audio::getInstance()->playCombo(selectedList.size());

	refreshScore();
	adjustMatrix();

	if (isEnded()){
		//////////////////////////////////////////////////////////////////////////
		//m_layer->floatLeftStarMsg(getLeftStarNum());//通知layer弹出剩余星星的信息
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelNoMoreMove, EventArg1::create(getLeftStarNum())));
		//////////////////////////////////////////////////////////////////////////

		CCLOG("ENDED");
	}
}

int StarMatrix::impl::getLeftStarNum()
{
	int ret = 0;
	for (int i = 0; i < ROW_NUM; i++){
		for (int j = 0; j < COL_NUM; j++){
			if (stars[i][j] == nullptr)
				continue;
			ret++;
		}
	}
	return ret;
}

void StarMatrix::impl::refreshScore()
{
	SingletonContainer::instance()->get<GameData>()->updateCurrentScoreWith(selectedList.size());
}

void StarMatrix::impl::adjustMatrix()
{
	//垂直方向调整
	for (int i = ROW_NUM - 1; i >= 0; i--){
		for (int j = COL_NUM - 1; j >= 0; j--){
			if (stars[i][j] == nullptr){
				int up = i;
				int dis = 0;
				while (stars[up][j] == nullptr){
					dis++;
					up--;
					if (up < 0){
						break;
					}
				}

				for (int begin_i = i - dis; begin_i >= 0; begin_i--){
					if (stars[begin_i][j] == nullptr)
						continue;
					Star* s = stars[begin_i + dis][j] = stars[begin_i][j];
					s->setIndex_ij(begin_i + dis, j);
					s->setDesPosition(getPositionByIndex(begin_i + dis, j));
					stars[begin_i][j] = nullptr;
				}
			}
			else{
				continue;
			}
		}
	}
	//水平方向调整
	for (int j = 0; j < COL_NUM; j++){
		if (stars[ROW_NUM - 1][j] == nullptr){
			int des = 0;
			int right = j;
			while (stars[ROW_NUM - 1][right] == nullptr){
				des++;
				right++;
			}
			for (int begin_i = 0; begin_i < ROW_NUM; begin_i++){
				for (int begin_j = j + des; begin_j < COL_NUM; begin_j++){
					if (stars[begin_i][begin_j] == nullptr)
						continue;
					Star* s = stars[begin_i][begin_j - des] = stars[begin_i][begin_j];
					s->setIndex_ij(begin_i, begin_j - des);
					s->setDesPosition(getPositionByIndex(begin_i, begin_j - des));
					stars[begin_i][begin_j] = nullptr;
				}
			}
		}
	}
}

cocos2d::Point StarMatrix::impl::getPositionByIndex(int i, int j)
{
	float x = j * Star::STAR_WIDTH + Star::STAR_WIDTH / 2;
	float y = (COL_NUM - i)*Star::STAR_HEIGHT - Star::STAR_HEIGHT / 2;
	return cocos2d::Point(x, y);
}

bool StarMatrix::impl::isEnded()
{
	bool bRet = true;
	for (int i = 0; i < ROW_NUM; i++){
		for (int j = 0; j < COL_NUM; j++){
			if (stars[i][j] == nullptr)
				continue;
			int curColor = stars[i][j]->getColor();
			//上
			if (i - 1 >= 0 && stars[i - 1][j] != nullptr && stars[i - 1][j]->getColor() == curColor)
				return false;
			//下
			if (i + 1 < ROW_NUM && stars[i + 1][j] != nullptr && stars[i + 1][j]->getColor() == curColor)
				return false;
			//左
			if (j - 1 >= 0 && stars[i][j - 1] != nullptr && stars[i][j - 1]->getColor() == curColor)
				return false;
			//右
			if (j + 1 < COL_NUM && stars[i][j + 1] != nullptr && stars[i][j + 1]->getColor() == curColor)
				return false;
		}
	}
	return bRet;
}

void StarMatrix::impl::update(float delta)
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

void StarMatrix::impl::clearMatrixOneByOne()
{
	for (int i = 0; i < ROW_NUM; i++){
		for (int j = 0; j < COL_NUM; j++){
			if (stars[i][j] == nullptr)
				continue;
			showStarParticleEffect(stars[i][j]->getColor(), stars[i][j]->getPosition(), m_node_underlying);
			stars[i][j]->removeFromParentAndCleanup(true);
			stars[i][j] = nullptr;
			return;
		}
	}
	//能够执行到这一句说明Matrix全为空，不在需要清空
	needClear = false;

	//转到下一关或者弹出结束游戏的窗口
	auto game_data = SingletonContainer::instance()->get<GameData>();
	if (game_data->getCurrentScore() >= game_data->getTargetScore()){
		game_data->levelUp();

		//////////////////////////////////////////////////////////////////////////
		//m_layer->gotoNextLevel();
		//m_layerGotoNextLevel();
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelUp));
		//////////////////////////////////////////////////////////////////////////
	}
	else{
		//////////////////////////////////////////////////////////////////////////
		//m_layer->gotoGameOver();
		//m_layerGotoGameOver();
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::GameOver));
		//////////////////////////////////////////////////////////////////////////
		CCLOG("GAME OVER");
	}
}

StarMatrix::StarMatrix(GameObject *game_object) : Script("StarMatrix", game_object), pimpl(new impl)
{
	//pimpl->m_node_underlying = game_object->addComponent<DisplayNode>()->initAs<LegacyStarMatrix>();
	pimpl->m_node_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	pimpl->init();
}

StarMatrix::~StarMatrix()
{

}

void StarMatrix::update(const std::time_t& time_ms)
{
	pimpl->update(time_ms);
}
