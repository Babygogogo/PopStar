#include <list>
#include <array>
#include <vector>
#include <functional>

#include "StarMatrixScript.h"
#include "StarScript.h"
#include "StarParticleScript.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Actor/SequentialInvoker.h"
#include "../Actor/Actor.h"
#include "../Utilities/SingletonContainer.h"
#include "../../Common/GameData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/BaseEventData.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "../GameLogic/GameLogic.h"
#include "../Audio/Audio.h"

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StarMatrixScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StarMatrixScript::StarMatrixScriptImpl
{
	StarMatrixScriptImpl(StarMatrixScript *visitor);
	~StarMatrixScriptImpl();

	void registerAsEventListeners();
	void unregisterAsEventListeners();

	void onTouch(const cocos2d::Point& p);

	bool isRowNumValid(int row_num) const;
	bool isColNumValid(int col_num) const;

	bool isNoMoreMove() const;
	int countStarsLeft() const;
	StarScript* getStarByPoint(const cocos2d::Point& position) const;
	cocos2d::Point getStarDefaultPosition(int row_num, int col_num) const;

	std::list<StarScript*> findGroupingStars(StarScript* touched_star);
	void iterateNeighborStars(int row_num, int col_num, std::function<void(StarScript*)> &&callback);
	void explode(StarScript* star);
	void explodeGroupingStars(std::list<StarScript*> &&group_stars);
	void shrink();
	void moveColumnsDownward();
	void moveColumnsLeftward();
	void explodeAllLeftStars();

	void update(float delta);

	static bool s_IsStaticInitialized;
	static int s_RowCount;
	static int s_ColumnCount;
	static std::vector<std::array<int, 2>> s_NeighborIndexOffsets;

	StarMatrixScript * m_Visitor{ nullptr };
	cocos2d::Node *m_node_underlying{ nullptr };
	SequentialInvoker *m_invoker{ nullptr };
	std::vector<std::vector<StarScript*>> m_StarScripts = {};
	std::vector<std::vector<StarScript*>> m_initial_stars = {};
};

bool StarMatrixScript::StarMatrixScriptImpl::s_IsStaticInitialized{ false };
int StarMatrixScript::StarMatrixScriptImpl::s_RowCount{};
int StarMatrixScript::StarMatrixScriptImpl::s_ColumnCount{};
std::vector<std::array<int, 2>> StarMatrixScript::StarMatrixScriptImpl::s_NeighborIndexOffsets{};

StarMatrixScript::StarMatrixScriptImpl::StarMatrixScriptImpl(StarMatrixScript *visitor) : m_Visitor{ visitor }
{
}

StarMatrixScript::StarMatrixScriptImpl::~StarMatrixScriptImpl()
{
	unregisterAsEventListeners();
}

void StarMatrixScript::StarMatrixScriptImpl::registerAsEventListeners()
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

void StarMatrixScript::StarMatrixScriptImpl::unregisterAsEventListeners()
{
	cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_node_underlying);

	if (auto& singleton_container = SingletonContainer::getInstance())
		singleton_container->get<IEventDispatcher>()->deleteListener(this);
}

void StarMatrixScript::StarMatrixScriptImpl::onTouch(const cocos2d::Point& p)
{
	explodeGroupingStars(findGroupingStars(getStarByPoint(p)));
}

bool StarMatrixScript::StarMatrixScriptImpl::isRowNumValid(int row_num) const
{
	return row_num >= 0 && row_num < s_RowCount;
}

bool StarMatrixScript::StarMatrixScriptImpl::isColNumValid(int col_num) const
{
	return col_num >= 0 && col_num < s_ColumnCount;
}

StarScript* StarMatrixScript::StarMatrixScriptImpl::getStarByPoint(const cocos2d::Point& position) const
{
	auto row_num = static_cast<int>(position.y / StarScript::HEIGHT);
	auto col_num = static_cast<int>(position.x / StarScript::WIDTH);

	if (isRowNumValid(row_num) && isColNumValid(col_num))
		return m_StarScripts[row_num][col_num];

	return nullptr;
}

std::list<StarScript*> StarMatrixScript::StarMatrixScriptImpl::findGroupingStars(StarScript* touched_star)
{
	if (!touched_star) return{};

	touched_star->setSelected(true);
	auto group_stars = std::list < StarScript* > {touched_star};

	for (auto current_star : group_stars)
		iterateNeighborStars(current_star->getRowNum(), current_star->getColNum(), [&group_stars, current_star](StarScript* neighbor_star){
		if (!neighbor_star->isSelected() && neighbor_star->canGroupWith(current_star)){
			neighbor_star->setSelected(true);
			group_stars.push_back(neighbor_star);
		}
	});

	for (auto &star : group_stars)
		star->setSelected(false);

	return group_stars;
}

void StarMatrixScript::StarMatrixScriptImpl::iterateNeighborStars(int row_num, int col_num, std::function<void(StarScript*)> &&callback)
{
	for (auto neighbor_index : s_NeighborIndexOffsets){
		auto n_row_num = row_num + neighbor_index[0];
		auto n_col_num = col_num + neighbor_index[1];
		if (isRowNumValid(n_row_num) && isColNumValid(n_col_num) && m_StarScripts[n_row_num][n_col_num])
			callback(m_StarScripts[n_row_num][n_col_num]);
	}
}

void StarMatrixScript::StarMatrixScriptImpl::explode(StarScript* star)
{
	if (!star)
		return;

	auto particleActor = SingletonContainer::getInstance()->get<GameLogic>()->createActor("Actors\\StarParticle.xml");
	particleActor->getComponent<StarParticleScript>()->show(star);
	m_Visitor->m_Actor.lock()->addChild(std::move(particleActor));

	m_StarScripts[star->getRowNum()][star->getColNum()] = nullptr;
	star->setVisible(false);
}

void StarMatrixScript::StarMatrixScriptImpl::explodeGroupingStars(std::list<StarScript*> &&group_stars)
{
	if (group_stars.size() <= 1)
		return;

	for (auto &star : group_stars)
		explode(star);
	Audio::getInstance()->playPop();

	shrink();

	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->get<GameData>()->updateCurrentScoreWith(group_stars.size());
	singletonContainer->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataPlayerExplodedStars>(group_stars.size()));

	if (isNoMoreMove()){
		SingletonContainer::getInstance()->get<GameData>()->setStarsLeftNum(countStarsLeft());
		SingletonContainer::getInstance()->get<IEventDispatcher>()->dispatch(std::make_unique<EvtDataGeneric>(EventType::LevelNoMoreMove));
	}
}

int StarMatrixScript::StarMatrixScriptImpl::countStarsLeft() const
{
	auto stars_left = 0;
	for (const auto &star_row : m_StarScripts)
		for (const auto &star : star_row)
			if (star) stars_left++;

	return stars_left;
}

void StarMatrixScript::StarMatrixScriptImpl::shrink()
{
	moveColumnsDownward();
	moveColumnsLeftward();

	for (auto row_num = 0; row_num < s_RowCount; ++row_num)
		for (auto col_num = 0; col_num < s_ColumnCount; ++col_num){
			auto star = m_StarScripts[row_num][col_num];

			if (star && (star->getRowNum() != row_num || star->getColNum() != col_num)){
				star->setRowNum(row_num);
				star->setColNum(col_num);

				auto position = getStarDefaultPosition(row_num, col_num);
				star->moveTo(position.x, position.y);
			}
		}
}

void StarMatrixScript::StarMatrixScriptImpl::moveColumnsDownward()
{
	for (auto col_num = 0; col_num < s_ColumnCount; ++col_num){
		auto row_offset = 1;
		for (auto row_num = 0; row_num < s_RowCount; ++row_num){
			if (m_StarScripts[row_num][col_num])
				continue;

			while (isRowNumValid(row_num + row_offset) && !m_StarScripts[row_num + row_offset][col_num])
				++row_offset;

			auto non_null_row_num = row_num + row_offset;
			if (!isRowNumValid(non_null_row_num))
				break;

			m_StarScripts[row_num][col_num] = m_StarScripts[non_null_row_num][col_num];
			m_StarScripts[non_null_row_num][col_num] = nullptr;
		}
	}
}

void StarMatrixScript::StarMatrixScriptImpl::moveColumnsLeftward()
{
	auto col_offset = 1;
	for (auto col_num = 0; col_num < s_ColumnCount; ++col_num){
		if (m_StarScripts[0][col_num])
			continue;

		while (isColNumValid(col_num + col_offset) && !m_StarScripts[0][col_num + col_offset])
			++col_offset;

		auto non_null_col_num = col_num + col_offset;
		if (!isColNumValid(non_null_col_num))
			break;

		for (auto row_num = 0; row_num < s_RowCount && m_StarScripts[row_num][non_null_col_num]; ++row_num){
			m_StarScripts[row_num][col_num] = m_StarScripts[row_num][non_null_col_num];
			m_StarScripts[row_num][non_null_col_num] = nullptr;
		}
	}
}

cocos2d::Point StarMatrixScript::StarMatrixScriptImpl::getStarDefaultPosition(int row_num, int col_num) const
{
	return cocos2d::Point(col_num * StarScript::WIDTH + StarScript::WIDTH / 2, row_num * StarScript::HEIGHT + StarScript::HEIGHT / 2);
}

bool StarMatrixScript::StarMatrixScriptImpl::isNoMoreMove() const
{
	for (auto col_num = 0; col_num < s_ColumnCount; ++col_num){
		for (auto row_num = 0; row_num < s_RowCount; ++row_num){
			if (!m_StarScripts[row_num][col_num])
				break;

			if ((isRowNumValid(row_num + 1) && m_StarScripts[row_num + 1][col_num] && m_StarScripts[row_num + 1][col_num]->canGroupWith(m_StarScripts[row_num][col_num]))
				|| (isColNumValid(col_num + 1) && m_StarScripts[row_num][col_num + 1] && m_StarScripts[row_num][col_num + 1]->canGroupWith(m_StarScripts[row_num][col_num])))
				return false;
		}
	}

	return true;
}

void StarMatrixScript::StarMatrixScriptImpl::explodeAllLeftStars()
{
	for (auto &star_row : m_StarScripts)
		for (auto &star : star_row)
			if (star)
				m_invoker->addFiniteTimeAction(cocos2d::Sequence::create(
				cocos2d::DelayTime::create(0.02f), cocos2d::CallFunc::create([this, star]{explode(star); }), nullptr));
}

//////////////////////////////////////////////////////////////////////////
//Implementation of StarMatrixScript.
//////////////////////////////////////////////////////////////////////////
StarMatrixScript::StarMatrixScript() : pimpl{ std::make_unique<StarMatrixScriptImpl>(this) }
{
}

StarMatrixScript::~StarMatrixScript()
{
}

void StarMatrixScript::reset()
{
	//Randomize the star matrix.
	for (auto row_num = 0; row_num < pimpl->s_RowCount; ++row_num)
		for (auto col_num = 0; col_num < pimpl->s_ColumnCount; ++col_num){
			pimpl->m_StarScripts[row_num][col_num] = pimpl->m_initial_stars[row_num][col_num];

			auto position = pimpl->getStarDefaultPosition(row_num, col_num);
			pimpl->m_StarScripts[row_num][col_num]->randomize(row_num, col_num, position.x, position.y);
		}

	pimpl->registerAsEventListeners();
}

const std::string & StarMatrixScript::getType() const
{
	return Type;
}

bool StarMatrixScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	if (!pimpl->s_IsStaticInitialized){
		pimpl->s_IsStaticInitialized = true;

		//Read the size data of the matrix from the xml.
		auto sizeElement = xmlElement->FirstChildElement("Size");
		pimpl->s_RowCount = sizeElement->IntAttribute("Row");
		pimpl->s_ColumnCount = sizeElement->IntAttribute("Column");

		//Read the neighbor index offsets from the xml.
		auto offsetElement = xmlElement->FirstChildElement("NeighborIndexOffset");
		auto offset = offsetElement->FirstChildElement();
		while (offset){
			std::array<int, 2> offsetPair{ { offset->IntAttribute("Row"), offset->IntAttribute("Column") } };
			pimpl->s_NeighborIndexOffsets.emplace_back(std::move(offsetPair));
			offset = offset->NextSiblingElement();
		}
	}

	return true;
}

void StarMatrixScript::vPostInit()
{
	auto actor = m_Actor.lock();
	pimpl->m_node_underlying = actor->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Layer>();

	pimpl->m_invoker = actor->getComponent<SequentialInvoker>().get();
	pimpl->m_invoker->setInvokeContinuously(true);

	//Resize the matrix.
	pimpl->m_StarScripts.resize(pimpl->s_RowCount);
	for (auto &row : pimpl->m_StarScripts)
		row.resize(pimpl->s_ColumnCount, nullptr);
	pimpl->m_initial_stars.resize(pimpl->s_RowCount);
	for (auto &row : pimpl->m_initial_stars)
		row.resize(pimpl->s_ColumnCount, nullptr);

	//Add stars.
	auto gameLogic = SingletonContainer::getInstance()->get<GameLogic>();
	for (auto row_num = 0; row_num < pimpl->s_RowCount; ++row_num)
		for (auto col_num = 0; col_num < pimpl->s_ColumnCount; ++col_num){
			auto starActor = gameLogic->createActor("Actors\\Star.xml");
			pimpl->m_StarScripts[row_num][col_num] = pimpl->m_initial_stars[row_num][col_num] = starActor->getComponent<StarScript>().get();
			actor->addChild(std::move(starActor));
		}
}

const std::string StarMatrixScript::Type = "StarMatrixScript";
