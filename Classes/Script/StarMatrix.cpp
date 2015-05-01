#include "StarMatrix.h"
#include "Star.h"
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

#include <list>
#include <array>
#include <functional>

struct StarMatrix::impl
{
	impl(GameObject *game_object):m_game_object(game_object){};
	~impl(){};

public:
	void addStars();
	void randomize();
	void registerAsEventListeners();
	void unregisterAsEventListeners();

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

	const static int ROWS_TOTAL = 10;
	const static int COLS_TOTAL = 10;
	const static std::array<std::array<int, 2>, 4> NEIGHBOR_INDEXES;

	GameObject *m_game_object{ nullptr };
	cocos2d::Node *m_node_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
	std::array<std::array<Star*, COLS_TOTAL>, ROWS_TOTAL> m_stars = {};
	std::array<std::array<Star*, COLS_TOTAL>, ROWS_TOTAL> m_initial_stars = {};
};

const std::array<std::array<int, 2>, 4> StarMatrix::impl::NEIGHBOR_INDEXES = { { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };

void StarMatrix::impl::addStars()
{
	for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num)
		for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
			auto star_object = GameObject::create("Star");
			m_stars[row_num][col_num] = m_initial_stars[row_num][col_num] = star_object->addComponent<Star>();
			m_game_object->addChild(std::move(star_object));
		}
}

void StarMatrix::impl::randomize()
{
	for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num)
		for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num)
			(m_stars[row_num][col_num] = m_initial_stars[row_num][col_num])->reset(row_num, col_num);
}

void StarMatrix::impl::registerAsEventListeners()
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		onTouch(cocos2d::Director::getInstance()->convertToGL(touch->getLocationInView()));
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_node_underlying);

	SingletonContainer::instance()->get<::EventDispatcher>()->registerListener(EventType::LevelResultEnded, this, [this](::Event*){
		clearStarsOneByOne();
		unregisterAsEventListeners();
	});
}

void StarMatrix::impl::unregisterAsEventListeners()
{
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_node_underlying);

	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
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
	auto row_num = ROWS_TOTAL - 1 - static_cast<int>(position.y / Star::HEIGHT);
	auto col_num = static_cast<int>(position.x / Star::WIDTH);

	if (isRowNumValid(row_num) && isColNumValid(col_num))
		return m_stars[row_num][col_num];
	
	return nullptr;
}

std::list<Star*> StarMatrix::impl::findGroupingStars(Star* touched_star)
{
	if (!touched_star) return {};

	touched_star->setSelected(true);
	auto group_stars = std::list < Star* > {touched_star};

	for (auto current_star : group_stars)
		iterateNeighborStars(current_star->getRowNum(), current_star->getColNum(), [&group_stars, current_star](Star* neighbor_star){
			if (!neighbor_star->isSelected() && neighbor_star->canGroupWith(current_star)){
				neighbor_star->setSelected(true);
				group_stars.push_back(neighbor_star);
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

	showStarParticleEffect(star->getColorNum(), { star->getPositionX(), star->getPositionY() }, m_node_underlying);
	m_stars[star->getRowNum()][star->getColNum()] = nullptr;
	star->setVisible(false);
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
			if (star && (star->getRowNum() != row_num || star->getColNum() != col_num))
				star->moveTo(row_num, col_num);
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
	return cocos2d::Point( col_num * Star::WIDTH + Star::WIDTH / 2,
		(ROWS_TOTAL - row_num) * Star::HEIGHT - Star::HEIGHT / 2 );
}

bool StarMatrix::impl::isNoMoreMove() const
{
	for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
		for (auto row_num = ROWS_TOTAL - 1; row_num >= 0; --row_num){
			if (!m_stars[row_num][col_num])
				break;

			if (	(isRowNumValid(row_num - 1) && m_stars[row_num - 1][col_num] && m_stars[row_num - 1][col_num]->canGroupWith(m_stars[row_num][col_num]))
				||	(isColNumValid(col_num + 1) && m_stars[row_num][col_num + 1] && m_stars[row_num][col_num + 1]->canGroupWith(m_stars[row_num][col_num])))
			return false;
		}
	}

	return true;
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

StarMatrix::StarMatrix(GameObject *game_object) : Script("StarMatrix", game_object), pimpl(new impl(game_object))
{
	pimpl->m_node_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();
	(pimpl->m_invoker = game_object->addComponent<SequentialInvoker>())->setInvokeContinuously(true);	

	pimpl->addStars();
}

StarMatrix::~StarMatrix()
{
	if (auto singleton_container = SingletonContainer::instance())
		singleton_container->get<EventDispatcher>()->deleteListener(this);
}

void StarMatrix::reset()
{
	pimpl->randomize();
	pimpl->registerAsEventListeners();
}
