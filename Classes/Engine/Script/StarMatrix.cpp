#include "StarMatrix.h"
#include "Star.h"
#include "StarParticleEffect.h"
#include "../Actor/DisplayNode.h"
#include "../Actor/SequentialInvoker.h"
#include "../Actor/Actor.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "cocos2d.h"
#include "../Audio/Audio.h"

#include <list>
#include <array>
#include <functional>

struct StarMatrix::impl
{
	impl(Actor *game_object);
	~impl();

	void addStars();
	void randomize();
	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void onTouch(const cocos2d::Point& p);

	bool isRowNumValid(int row_num) const;
	bool isColNumValid(int col_num) const;

	bool isNoMoreMove() const;
	int countStarsLeft() const;
	Star* getStarByPoint(const cocos2d::Point& position) const;
	cocos2d::Point getStarDefaultPosition(int row_num, int col_num) const;

	std::list<Star*> findGroupingStars(Star* touched_star);
	void iterateNeighborStars(int row_num, int col_num, std::function<void(Star*)> &&callback);
	void explode(Star* star);
	void explodeGroupingStars(std::list<Star*> &&group_stars);
	void shrink();
	void moveColumnsDownward();
	void moveColumnsLeftward();
	void explodeAllLeftStars();

	void update(float delta);

	const static int ROWS_TOTAL = 10;
	const static int COLS_TOTAL = 10;
	const static std::array<std::array<int, 2>, 4> NEIGHBOR_INDEXES;

	Actor *m_game_object{ nullptr };
	cocos2d::Node *m_node_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
	std::array<std::array<Star*, COLS_TOTAL>, ROWS_TOTAL> m_stars = {};
	std::array<std::array<Star*, COLS_TOTAL>, ROWS_TOTAL> m_initial_stars = {};
};

const std::array<std::array<int, 2>, 4> StarMatrix::impl::NEIGHBOR_INDEXES = { { { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 } } };

StarMatrix::impl::impl(Actor *game_object) :m_game_object(game_object)
{
	m_node_underlying = game_object->addComponent<DisplayNode>()->initAs<cocos2d::Layer>();

	m_invoker = game_object->addComponent<SequentialInvoker>().get();
	m_invoker->setInvokeContinuously(true);

	addStars();
}

StarMatrix::impl::~impl()
{
	unregisterAsEventListeners();
}

void StarMatrix::impl::addStars()
{
	for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num)
		for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
			auto star_object = std::make_shared<Actor>();
			m_stars[row_num][col_num] = m_initial_stars[row_num][col_num] = star_object->addComponent<Star>().get();
			m_game_object->addChild(std::move(star_object));
		}
}

void StarMatrix::impl::randomize()
{
	for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num)
		for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
			m_stars[row_num][col_num] = m_initial_stars[row_num][col_num];

			auto position = getStarDefaultPosition(row_num, col_num);
			m_stars[row_num][col_num]->randomize(row_num, col_num, position.x, position.y);
		}
}

void StarMatrix::impl::registerAsEventListeners()
{
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		onTouch(cocos2d::Director::getInstance()->convertToGL(touch->getLocationInView()));
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_node_underlying);

	SingletonContainer::getInstance()->get<IEventDispatcher>()->registerListener(EventType::LevelSummaryLabelDisappeared, this, [this](::BaseEventData*){
		explodeAllLeftStars();
		m_invoker->addCallback([]{SingletonContainer::getInstance()->get<GameData>()->levelEnd(); });
		unregisterAsEventListeners();
	});
}

void StarMatrix::impl::unregisterAsEventListeners()
{
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_node_underlying);

	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void StarMatrix::impl::onTouch(const cocos2d::Point& p)
{
	explodeGroupingStars(findGroupingStars(getStarByPoint(p)));
}

bool StarMatrix::impl::isRowNumValid(int row_num) const
{
	return row_num >= 0 && row_num < ROWS_TOTAL;
}

bool StarMatrix::impl::isColNumValid(int col_num) const
{
	return col_num >= 0 && col_num < COLS_TOTAL;
}

Star* StarMatrix::impl::getStarByPoint(const cocos2d::Point& position) const
{
	auto row_num = static_cast<int>(position.y / Star::HEIGHT);
	auto col_num = static_cast<int>(position.x / Star::WIDTH);

	if (isRowNumValid(row_num) && isColNumValid(col_num))
		return m_stars[row_num][col_num];

	return nullptr;
}

std::list<Star*> StarMatrix::impl::findGroupingStars(Star* touched_star)
{
	if (!touched_star) return{};

	touched_star->setSelected(true);
	auto group_stars = std::list < Star* > {touched_star};

	for (auto current_star : group_stars)
		iterateNeighborStars(current_star->getRowNum(), current_star->getColNum(), [&group_stars, current_star](Star* neighbor_star){
		if (!neighbor_star->isSelected() && neighbor_star->canGroupWith(current_star)){
			neighbor_star->setSelected(true);
			group_stars.push_back(neighbor_star);
		}
	});

	for (auto &star : group_stars)
		star->setSelected(false);

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

	auto particle_effect = std::make_shared<Actor>();
	particle_effect->addComponent<StarParticleEffect>()->reset(star);
	m_game_object->addChild(std::move(particle_effect));

	m_stars[star->getRowNum()][star->getColNum()] = nullptr;
	star->setVisible(false);
}

void StarMatrix::impl::explodeGroupingStars(std::list<Star*> &&group_stars)
{
	if (group_stars.size() <= 1)
		return;

	for (auto &star : group_stars)
		explode(star);
	Audio::getInstance()->playPop();

	shrink();
	SingletonContainer::getInstance()->get<GameData>()->updateCurrentScoreWith(group_stars.size());
	//	SingletonContainer::getInstance()->get<IEventDispatcher>()->dispatch(LegacyEvent::create(LegacyEventType::PlayerExplodedStars, EventArg1::create(group_stars.size())));
	SingletonContainer::getInstance()->get<IEventDispatcher>()->dispatch(std::make_unique<EvtDataPlayerExplodedStars>(group_stars.size()));

	if (isNoMoreMove()){
		SingletonContainer::getInstance()->get<GameData>()->setStarsLeftNum(countStarsLeft());
		SingletonContainer::getInstance()->get<IEventDispatcher>()->dispatch(std::make_unique<EvtDataGeneric>(EventType::LevelNoMoreMove));
	}
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
				star->setRowNum(row_num);
				star->setColNum(col_num);

				auto position = getStarDefaultPosition(row_num, col_num);
				star->moveTo(position.x, position.y);
			}
		}
}

void StarMatrix::impl::moveColumnsDownward()
{
	for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
		auto row_offset = 1;
		for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num){
			if (m_stars[row_num][col_num])
				continue;

			while (isRowNumValid(row_num + row_offset) && !m_stars[row_num + row_offset][col_num])
				++row_offset;

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
		if (m_stars[0][col_num])
			continue;

		while (isColNumValid(col_num + col_offset) && !m_stars[0][col_num + col_offset])
			++col_offset;

		auto non_null_col_num = col_num + col_offset;
		if (!isColNumValid(non_null_col_num))
			break;

		for (auto row_num = 0; row_num < ROWS_TOTAL && m_stars[row_num][non_null_col_num]; ++row_num){
			m_stars[row_num][col_num] = m_stars[row_num][non_null_col_num];
			m_stars[row_num][non_null_col_num] = nullptr;
		}
	}
}

cocos2d::Point StarMatrix::impl::getStarDefaultPosition(int row_num, int col_num) const
{
	return cocos2d::Point(col_num * Star::WIDTH + Star::WIDTH / 2, row_num * Star::HEIGHT + Star::HEIGHT / 2);
}

bool StarMatrix::impl::isNoMoreMove() const
{
	for (auto col_num = 0; col_num < COLS_TOTAL; ++col_num){
		for (auto row_num = 0; row_num < ROWS_TOTAL; ++row_num){
			if (!m_stars[row_num][col_num])
				break;

			if ((isRowNumValid(row_num + 1) && m_stars[row_num + 1][col_num] && m_stars[row_num + 1][col_num]->canGroupWith(m_stars[row_num][col_num]))
				|| (isColNumValid(col_num + 1) && m_stars[row_num][col_num + 1] && m_stars[row_num][col_num + 1]->canGroupWith(m_stars[row_num][col_num])))
				return false;
		}
	}

	return true;
}

void StarMatrix::impl::explodeAllLeftStars()
{
	for (auto &star_row : m_stars)
		for (auto &star : star_row)
			if (star)
				m_invoker->addFiniteTimeAction(cocos2d::Sequence::create(
				cocos2d::DelayTime::create(0.02f), cocos2d::CallFunc::create([this, star]{explode(star); }), nullptr));
}

StarMatrix::StarMatrix(Actor *game_object) : BaseScriptComponent("StarMatrix", game_object), pimpl{ std::make_unique<impl>(game_object) }
{
}

StarMatrix::~StarMatrix()
{
}

void StarMatrix::reset()
{
	pimpl->randomize();
	pimpl->registerAsEventListeners();
}

const std::string & StarMatrix::getType() const
{
	return Type;
}

const std::string StarMatrix::Type = "StarMatrixScript";
