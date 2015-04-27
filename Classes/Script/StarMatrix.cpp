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

#include <list>
#include <array>
#include <random>
#include <functional>

struct StarMatrix::impl
{
	impl(){};
	~impl(){};

public:
	void randomizeMatrix();
	void registerTouchListener();
	void onTouch(const cocos2d::Point& p);

	bool isRowNumValid(int row_num);
	bool isColNumValid(int col_num);

	cocos2d::Point getStarDefaultPosition(int row_num, int col_num);
	Star* getStarByTouch(const cocos2d::Point& position);
	std::list<Star*> findGroupingStars(Star* touched_star);
	void iterateNeighborStars(int row_num, int col_num, std::function<void(Star*)> &&callback);
	void explodeGroupingStars(std::list<Star*> &&group_stars);
	void adjustMatrix();
	void updateScoreByNumExplodedStars(int num);
	bool isNoMoreMove();
	void clearMatrixOneByOne();
	int countStarsLeft();

	void update(float delta);

public:
	const static int ROWS_TOTAL = 10;
	const static int COLS_TOTAL = 10;
	const static std::array<std::array<int, 2>, 4> NEIGHBOR_INDEXES;
	const static float ONE_CLEAR_TIME;

	cocos2d::Node *m_node_underlying{ nullptr };
	std::array<std::array<Star*, COLS_TOTAL>, ROWS_TOTAL> stars;
//	std::list<Star*> selectedList;
	bool needClear{ false };
	float clearSumTime{ 0 };
};

const std::array<std::array<int, 2>, 4> StarMatrix::impl::NEIGHBOR_INDEXES = { { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };
const float StarMatrix::impl::ONE_CLEAR_TIME = 0.05f;

void StarMatrix::impl::randomizeMatrix()
{
	std::random_device rd;
	std::default_random_engine e(rd());
	std::uniform_int_distribution<> u_color(0, Star::COLOR_MAX_NUM - 1);
	std::uniform_int_distribution<> u_position(-300, 300);

	for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num){
		for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
			auto star = Star::create(u_color(e));
			star->setMatrixIndex(row_num, col_num);

			auto position = getStarDefaultPosition(row_num, col_num);
			star->setPosition(position + cocos2d::Point(u_position(e), u_position(e)));
			star->setDesPosition(position);

			stars[row_num][col_num] = star;
			m_node_underlying->addChild(star);
		}
	}
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
	explodeGroupingStars(findGroupingStars(getStarByTouch(p)));
}

bool StarMatrix::impl::isRowNumValid(int row_num)
{
	return row_num >= 0 && row_num < ROWS_TOTAL;
}

bool StarMatrix::impl::isColNumValid(int col_num)
{
	return col_num >= 0 && col_num < COLS_TOTAL;
}

Star* StarMatrix::impl::getStarByTouch(const cocos2d::Point& position)
{
	auto row_num = ROWS_TOTAL - 1 - static_cast<int>(position.y / Star::STAR_HEIGHT);
	auto col_num = static_cast<int>(position.x / Star::STAR_WIDTH);

	if (isRowNumValid(row_num) && isColNumValid(col_num))
		return stars[row_num][col_num];
	
	return nullptr;
}

std::list<Star*> StarMatrix::impl::findGroupingStars(Star* touched_star)
{
	if (!touched_star) return {};

	touched_star->setSelected(true);
	auto group_stars = std::list < Star* > {touched_star};

	for (auto star : group_stars)
		iterateNeighborStars(star->getRowNum(), star->getColNum(), [&group_stars, star](Star* n_star){
			if (!n_star->isSelected() && n_star->getColor() == star->getColor()){
				n_star->setSelected(true);
				group_stars.push_back(n_star);
			}
		});

	return group_stars;
}

void StarMatrix::impl::iterateNeighborStars(int row_num, int col_num, std::function<void(Star*)> &&callback)
{
	for (auto neighbor_index : NEIGHBOR_INDEXES){
		auto n_row_num = row_num + neighbor_index[0];
		auto n_col_num = col_num + neighbor_index[1];
		if (isRowNumValid(n_row_num) && isColNumValid(n_col_num) && stars[n_row_num][n_col_num])
			callback(stars[n_row_num][n_col_num]);
	}
}

void StarMatrix::impl::explodeGroupingStars(std::list<Star*> &&group_stars)
{
	if (group_stars.size() <= 1){
		if (!group_stars.empty())
			group_stars.front()->setSelected(false);
		return;
	}

	for (auto &star : group_stars){
		showStarParticleEffect(star->getColor(), star->getPosition(), m_node_underlying);
		stars[star->getRowNum()][star->getColNum()] = nullptr;
		star->removeFromParentAndCleanup(true);
	}

	showComboEffect(group_stars.size(), m_node_underlying);
	updateScoreByNumExplodedStars(group_stars.size());
	adjustMatrix();

	Audio::getInstance()->playCombo(group_stars.size());
	Audio::getInstance()->playPop();

	if (isNoMoreMove()){
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelNoMoreMove, EventArg1::create(countStarsLeft())));

		CCLOG("ENDED");
	}
}

int StarMatrix::impl::countStarsLeft()
{
	auto stars_left = 0;
	for (const auto &star_row : stars)
		for (const auto &star : star_row)
			if (star) stars_left++;

	return stars_left;
}

void StarMatrix::impl::updateScoreByNumExplodedStars(int num)
{
	SingletonContainer::instance()->get<GameData>()->updateCurrentScoreWith(num);
}

void StarMatrix::impl::adjustMatrix()
{
	//垂直方向调整
	for (int i = ROWS_TOTAL - 1; i >= 0; i--){
		for (int j = COLS_TOTAL - 1; j >= 0; j--){
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
					s->setMatrixIndex(begin_i + dis, j);
					s->setDesPosition(getStarDefaultPosition(begin_i + dis, j));
					stars[begin_i][j] = nullptr;
				}
			}
			else{
				continue;
			}
		}
	}
	//水平方向调整
	for (int j = 0; j < COLS_TOTAL; j++){
		if (stars[ROWS_TOTAL - 1][j] == nullptr){
			int des = 0;
			int right = j;
			while (stars[ROWS_TOTAL - 1][right] == nullptr){
				des++;
				if (++right >= COLS_TOTAL)	break;
			}
			for (int begin_i = 0; begin_i < ROWS_TOTAL; begin_i++){
				for (int begin_j = j + des; begin_j < COLS_TOTAL; begin_j++){
					if (stars[begin_i][begin_j] == nullptr)
						continue;
					Star* s = stars[begin_i][begin_j - des] = stars[begin_i][begin_j];
					s->setMatrixIndex(begin_i, begin_j - des);
					s->setDesPosition(getStarDefaultPosition(begin_i, begin_j - des));
					stars[begin_i][begin_j] = nullptr;
				}
			}
		}
	}
}

cocos2d::Point StarMatrix::impl::getStarDefaultPosition(int row_num, int col_num)
{
	return cocos2d::Point( col_num * Star::STAR_WIDTH + Star::STAR_WIDTH / 2,
		(ROWS_TOTAL - row_num) * Star::STAR_HEIGHT - Star::STAR_HEIGHT / 2 );
}

bool StarMatrix::impl::isNoMoreMove()
{
	bool bRet = true;
	for (int i = 0; i < ROWS_TOTAL; i++){
		for (int j = 0; j < COLS_TOTAL; j++){
			if (stars[i][j] == nullptr)
				continue;
			int curColor = stars[i][j]->getColor();
			//上
			if (i - 1 >= 0 && stars[i - 1][j] != nullptr && stars[i - 1][j]->getColor() == curColor)
				return false;
			//下
			if (i + 1 < ROWS_TOTAL && stars[i + 1][j] != nullptr && stars[i + 1][j]->getColor() == curColor)
				return false;
			//左
			if (j - 1 >= 0 && stars[i][j - 1] != nullptr && stars[i][j - 1]->getColor() == curColor)
				return false;
			//右
			if (j + 1 < COLS_TOTAL && stars[i][j + 1] != nullptr && stars[i][j + 1]->getColor() == curColor)
				return false;
		}
	}
	return bRet;
}

void StarMatrix::impl::update(float delta)
{
	for (int i = 0; i < ROWS_TOTAL; i++){
		for (int j = 0; j < COLS_TOTAL; j++){
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
	for (int i = 0; i < ROWS_TOTAL; i++){
		for (int j = 0; j < COLS_TOTAL; j++){
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

		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelUp));
	}
	else{
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::GameOver));
		CCLOG("GAME OVER");
	}
}

StarMatrix::StarMatrix(GameObject *game_object) : Script("StarMatrix", game_object), pimpl(new impl)
{
	pimpl->m_node_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	pimpl->randomizeMatrix();
	pimpl->registerTouchListener();
	SingletonContainer::instance()->get<::EventDispatcher>()->registerListener(EventType::LevelResultEnded, this,
		[this](::Event*){pimpl->needClear = true; });
}

StarMatrix::~StarMatrix()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void StarMatrix::update(const std::time_t& time_ms)
{
	pimpl->update(time_ms);
}
