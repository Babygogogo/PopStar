#include "StarMatrix.h"
#include "../GameObject/DisplayNode.h"
#include "../GameObject/SequentialInvoker.h"
#include "../GameObject/GameObject.h"
#include "../Common/SingletonContainer.h"
#include "../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/Event.h"
#include "../Event/EventArg1.h"

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

	bool isRowNumValid(int row_num) const;
	bool isColNumValid(int col_num) const;

	bool isNoMoreMove() const;
	int countStarsLeft() const;
	Star* getStarByTouch(const cocos2d::Point& position) const;
	cocos2d::Point getStarDefaultPosition(int row_num, int col_num) const;

	std::list<Star*> findGroupingStars(Star* touched_star);
	void iterateNeighborStars(int row_num, int col_num, std::function<void(Star*)> &&callback);
	void explode(Star* star);
	void explodeGroupingStars(std::list<Star*> &&group_stars);
	void shrink();
	void moveColumnsDownward();
	void moveColumnsLeftward();
	void clearStarsOneByOne();

	void update(float delta);

public:
	const static int ROWS_TOTAL = 10;
	const static int COLS_TOTAL = 10;
	const static std::array<std::array<int, 2>, 4> NEIGHBOR_INDEXES;

	cocos2d::Node *m_node_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
	std::array<std::array<Star*, COLS_TOTAL>, ROWS_TOTAL> m_stars;
};

const std::array<std::array<int, 2>, 4> StarMatrix::impl::NEIGHBOR_INDEXES = { { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };

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

			m_stars[row_num][col_num] = star;
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

bool StarMatrix::impl::isRowNumValid(int row_num) const
{
	return row_num >= 0 && row_num < ROWS_TOTAL;
}

bool StarMatrix::impl::isColNumValid(int col_num) const
{
	return col_num >= 0 && col_num < COLS_TOTAL;
}

Star* StarMatrix::impl::getStarByTouch(const cocos2d::Point& position) const
{
	auto row_num = ROWS_TOTAL - 1 - static_cast<int>(position.y / Star::STAR_HEIGHT);
	auto col_num = static_cast<int>(position.x / Star::STAR_WIDTH);

	if (isRowNumValid(row_num) && isColNumValid(col_num))
		return m_stars[row_num][col_num];
	
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
		if (isRowNumValid(n_row_num) && isColNumValid(n_col_num) && m_stars[n_row_num][n_col_num])
			callback(m_stars[n_row_num][n_col_num]);
	}
}

void StarMatrix::impl::explode(Star* star)
{
	if (!star)
		return;

	showStarParticleEffect(star->getColor(), star->getPosition(), m_node_underlying);
	m_stars[star->getRowNum()][star->getColNum()] = nullptr;
	star->removeFromParentAndCleanup(true);
}

void StarMatrix::impl::explodeGroupingStars(std::list<Star*> &&group_stars)
{
	if (group_stars.size() <= 1){
		if (!group_stars.empty())
			group_stars.front()->setSelected(false);
		return;
	}

	for (auto &star : group_stars)
		explode(star);

	showComboEffect(group_stars.size(), m_node_underlying);
	shrink();
	SingletonContainer::instance()->get<GameData>()->updateCurrentScoreWith(group_stars.size());

	Audio::getInstance()->playCombo(group_stars.size());
	Audio::getInstance()->playPop();

	if (isNoMoreMove())
		SingletonContainer::instance()->get<::EventDispatcher>()->dispatch(::Event::create(EventType::LevelNoMoreMove, EventArg1::create(countStarsLeft())));
}

int StarMatrix::impl::countStarsLeft() const
{
	auto stars_left = 0;
	for (const auto &star_row : m_stars)
		for (const auto &star : star_row)
			if (star) stars_left++;

	return stars_left;
}

void StarMatrix::impl::shrink()
{
	moveColumnsDownward();
	moveColumnsLeftward();

	for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num)
		for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
			auto star = m_stars[row_num][col_num];
			if (star && (star->getRowNum() != row_num || star->getColNum() != col_num)){
				star->setMatrixIndex(row_num, col_num);
				star->setDesPosition(getStarDefaultPosition(row_num, col_num));
			}
		}
}

void StarMatrix::impl::moveColumnsDownward()
{
	for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
		auto row_offset = -1;
		for (auto row_num = COLS_TOTAL - 1; row_num >= 0; --row_num){
			if (m_stars[row_num][col_num])
				continue;

			while (isRowNumValid(row_num + row_offset) && !m_stars[row_num + row_offset][col_num])
				--row_offset;

			auto non_null_row_num = row_num + row_offset;
			if (!isRowNumValid(non_null_row_num))
				break;

			m_stars[row_num][col_num] = m_stars[non_null_row_num][col_num];
			m_stars[non_null_row_num][col_num] = nullptr;
		}
	}
}

void StarMatrix::impl::moveColumnsLeftward()
{
	auto col_offset = 1;
	for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
		if (m_stars[ROWS_TOTAL - 1][col_num])
			continue;

		while (isColNumValid(col_num + col_offset) && !m_stars[ROWS_TOTAL - 1][col_num + col_offset])
			++col_offset;

		auto non_null_col_num = col_num + col_offset;
		if (!isColNumValid(non_null_col_num))
			break;

		for (auto row_num = ROWS_TOTAL - 1; row_num >= 0 && m_stars[row_num][non_null_col_num]; --row_num){
			m_stars[row_num][col_num] = m_stars[row_num][non_null_col_num];
			m_stars[row_num][non_null_col_num] = nullptr;
		}
	}
}

cocos2d::Point StarMatrix::impl::getStarDefaultPosition(int row_num, int col_num) const
{
	return cocos2d::Point( col_num * Star::STAR_WIDTH + Star::STAR_WIDTH / 2,
		(ROWS_TOTAL - row_num) * Star::STAR_HEIGHT - Star::STAR_HEIGHT / 2 );
}

bool StarMatrix::impl::isNoMoreMove() const
{
	for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
		for (auto row_num = ROWS_TOTAL - 1; row_num >= 0; --row_num){
			if (!m_stars[row_num][col_num])
				break;

			const auto current_color = m_stars[row_num][col_num]->getColor();
			if (	(isRowNumValid(row_num - 1) && m_stars[row_num - 1][col_num] && m_stars[row_num - 1][col_num]->getColor() == current_color)
				||	(isColNumValid(col_num + 1) && m_stars[row_num][col_num + 1] && m_stars[row_num][col_num + 1]->getColor() == current_color))
				return false;
		}
	}

	return true;
}

void StarMatrix::impl::update(float delta)
{
	for (int row_num = 0; row_num < ROWS_TOTAL; row_num++)
		for (int col_num = 0; col_num < COLS_TOTAL; col_num++)
			if (m_stars[row_num][col_num])
				m_stars[row_num][col_num]->updatePosition();
}

void StarMatrix::impl::clearStarsOneByOne()
{
	for (auto &star_row : m_stars)
		for (auto &star : star_row)
			if (star)
				m_invoker->addFiniteTimeAction(cocos2d::Sequence::create(
					cocos2d::DelayTime::create(0.02f), cocos2d::CallFunc::create([this, star]{explode(star); }), nullptr));

	m_invoker->addFiniteTimeAction(cocos2d::CallFunc::create([]{SingletonContainer::instance()->get<GameData>()->levelEnd(); }));
}

StarMatrix::StarMatrix(GameObject *game_object) : Script("StarMatrix", game_object), pimpl(new impl)
{
	pimpl->m_node_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();
	(pimpl->m_invoker = game_object->addComponent<SequentialInvoker>())->setInvokeContinuously(true);	

	pimpl->randomizeMatrix();
	pimpl->registerTouchListener();
	SingletonContainer::instance()->get<::EventDispatcher>()->registerListener(EventType::LevelResultEnded, this,
		[this](::Event*){pimpl->clearStarsOneByOne(); });
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
