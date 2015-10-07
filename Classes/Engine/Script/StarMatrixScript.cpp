#include <array>
#include <functional>
#include <list>
#include <vector>

#include "cocos2d.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

#include "StarMatrixScript.h"
#include "StarScript.h"
#include "StarParticleScript.h"
#include "../Actor/Actor.h"
#include "../Actor/BaseRenderComponent.h"
#include "../Actor/FiniteTimeActionComponent.h"
#include "../Audio/Audio.h"
#include "../Event/BaseEventData.h"
#include "../Event/EventDispatcher.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataGeneric.h"
#include "../Event/EvtDataLevelNoMoreMove.h"
#include "../Event/EvtDataPlayerExplodedStars.h"
#include "../GameLogic/GameLogic.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of StarMatrixScriptImpl.
//////////////////////////////////////////////////////////////////////////
struct StarMatrixScript::StarMatrixScriptImpl
{
	StarMatrixScriptImpl();
	~StarMatrixScriptImpl();

	void onTouch(const cocos2d::Point & position);
	void onLevelSummaryFinished(const IEventData & e);
	void onStartLevelLabelDisappeared(const IEventData & e);

	bool isRowIndexValid(int row_num) const;
	bool isColIndexValid(int col_num) const;

	bool isNoMoreMove() const;
	int countRemainingStars() const;
	cocos2d::Point getStarDefaultPosition(int rowIndex, int colIndex) const;
	StarScript * getStarWithPosition(int rowIndex, int colIndex, int rowOffset, int colOffset) const;

	void explode(StarScript* star);
	void removeEmptyBlocks();

	static int s_RowCount;
	static int s_ColumnCount;
	static std::vector<std::array<int, 2>> s_NeighborIndexOffsets;
	static float s_IntervalForExplodingRemainingStars;

	std::weak_ptr<Actor> m_Actor;
	std::vector<std::vector<StarScript*>> m_StarScripts;
	std::vector<std::vector<StarScript*>> m_initial_stars;
};

int StarMatrixScript::StarMatrixScriptImpl::s_RowCount{};
int StarMatrixScript::StarMatrixScriptImpl::s_ColumnCount{};
std::vector<std::array<int, 2>> StarMatrixScript::StarMatrixScriptImpl::s_NeighborIndexOffsets;
float StarMatrixScript::StarMatrixScriptImpl::s_IntervalForExplodingRemainingStars{};

StarMatrixScript::StarMatrixScriptImpl::StarMatrixScriptImpl()
{
}

StarMatrixScript::StarMatrixScriptImpl::~StarMatrixScriptImpl()
{
}

void StarMatrixScript::StarMatrixScriptImpl::onTouch(const cocos2d::Point & position)
{
	//////////////////////////////////////////////////////////////////////////
	//Find which star is touched.
	auto touchedStar = static_cast<StarScript*>(nullptr);
	auto rowIndex = static_cast<int>(position.y / StarScript::HEIGHT);
	auto colIndex = static_cast<int>(position.x / StarScript::WIDTH);
	if (isRowIndexValid(rowIndex) && isColIndexValid(colIndex))
		touchedStar = m_StarScripts[rowIndex][colIndex];

	//If can't find a star in the touched position, do nothing and return.
	if (!touchedStar)
		return;

	//////////////////////////////////////////////////////////////////////////
	//Find all the stars that can be grouped with the touched star.
	//Firstly, add the touched star into the group.
	auto groupStars = std::list < StarScript* > {touchedStar};
	touchedStar->setIsInGroup(true);

	//Secondly, iterate through all neighbor stars of the grouped stars to see if they can be grouped.
	for (const auto & currentStar : groupStars){
		for (const auto neighborIndexOffset : s_NeighborIndexOffsets){
			auto neighborStar = getStarWithPosition(currentStar->getRowIndex(), currentStar->getColIndex(), neighborIndexOffset[0], neighborIndexOffset[1]);

			//If the neighbor star can be grouped and not already in the group, group it.
			if (currentStar->canGroupWith(neighborStar) && !neighborStar->isInGroup()){
				neighborStar->setIsInGroup(true);
				groupStars.emplace_back(neighborStar);
			}
		}
	}

	//Thirdly, reset the is-in-group mark, or it may cause incorrectness in the next touches.
	for (auto &star : groupStars)
		star->setIsInGroup(false);

	//Finally, check if there are less than 2 stars in the group. If not so, do nothing and return.
	if (groupStars.size() < 2)
		return;

	//////////////////////////////////////////////////////////////////////////
	//Explode all the stars in the group.
	//Firstly, show visual explosion effect and playback sound effect.
	for (auto &star : groupStars)
		explode(star);
	Audio::getInstance()->playPop();

	//Secondly, remove the visual empty blocks in the star matrix.
	removeEmptyBlocks();

	//Thirdly, dispatch an event telling how many stars exploded.
	auto & singletonContainer = SingletonContainer::getInstance();
	singletonContainer->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataPlayerExplodedStars>(groupStars.size()));

	//Finally, check if there is no more move. If no, we will enter the level summary phase.
	if (isNoMoreMove()){
		//Stop listening to touch events.
		cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(m_Actor.lock()->getRenderComponent()->getSceneNode());

		//Dispatch an event saying that there is no more move.
		singletonContainer->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataLevelNoMoreMove>(countRemainingStars()));
	}
}

void StarMatrixScript::StarMatrixScriptImpl::onLevelSummaryFinished(const IEventData & e)
{
	auto actionComponent = m_Actor.lock()->getComponent<FiniteTimeActionComponent>();

	//Explode remaining stars.
	for (auto &star_row : m_StarScripts){
		for (auto &star : star_row)
			if (star)
				actionComponent->queueAction(cocos2d::Sequence::create(cocos2d::DelayTime::create(s_IntervalForExplodingRemainingStars), cocos2d::CallFunc::create([this, star]{explode(star); }), nullptr));
	}

	//Dispatch an event after the stars finish exploding.
	actionComponent->queueCallback([]{
		SingletonContainer::getInstance()->get<IEventDispatcher>()->vQueueEvent(std::make_unique<EvtDataGeneric>(EventType::RemainingStarsExploded));
	});
}

void StarMatrixScript::StarMatrixScriptImpl::onStartLevelLabelDisappeared(const IEventData & e)
{
	//Randomize the star matrix.
	for (auto row_num = 0; row_num < s_RowCount; ++row_num){
		for (auto col_num = 0; col_num < s_ColumnCount; ++col_num){
			m_StarScripts[row_num][col_num] = m_initial_stars[row_num][col_num];

			auto position = getStarDefaultPosition(row_num, col_num);
			m_StarScripts[row_num][col_num]->randomize(row_num, col_num, position.x, position.y);
		}
	}

	//Listen to touch events.
	auto listener = cocos2d::EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](cocos2d::Touch* touch, cocos2d::Event* event)->bool{
		onTouch(cocos2d::Director::getInstance()->convertToGL(touch->getLocationInView()));
		return true;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, m_Actor.lock()->getRenderComponent()->getSceneNode());

	//Play sound.
	Audio::getInstance()->playReadyGo();
}

bool StarMatrixScript::StarMatrixScriptImpl::isRowIndexValid(int row_num) const
{
	return row_num >= 0 && row_num < s_RowCount;
}

bool StarMatrixScript::StarMatrixScriptImpl::isColIndexValid(int col_num) const
{
	return col_num >= 0 && col_num < s_ColumnCount;
}

bool StarMatrixScript::StarMatrixScriptImpl::isNoMoreMove() const
{
	for (auto colIndex = 0; colIndex < s_ColumnCount; ++colIndex){
		for (auto rowIndex = 0; rowIndex < s_RowCount; ++rowIndex){
			//Check if there is star in {rowIndex, colIndex} of the matrix. If not, search for the next column.
			auto currentStar = m_StarScripts[rowIndex][colIndex];
			if (!currentStar)
				break;

			//Check if the current star can group with the upper or right star. If so, there is more moves and we simply return false.
			if (currentStar->canGroupWith(getStarWithPosition(rowIndex, colIndex, 1, 0)) || currentStar->canGroupWith(getStarWithPosition(rowIndex, colIndex, 0, 1)))
				return false;
		}
	}

	return true;
}

int StarMatrixScript::StarMatrixScriptImpl::countRemainingStars() const
{
	auto remainingStarsCount = 0;
	for (const auto & star_row : m_StarScripts)
		for (const auto & star : star_row)
			if (star)
				++remainingStarsCount;

	return remainingStarsCount;
}

cocos2d::Point StarMatrixScript::StarMatrixScriptImpl::getStarDefaultPosition(int rowIndex, int colIndex) const
{
	return cocos2d::Point(colIndex * StarScript::WIDTH + StarScript::WIDTH / 2, rowIndex * StarScript::HEIGHT + StarScript::HEIGHT / 2);
}

StarScript * StarMatrixScript::StarMatrixScriptImpl::getStarWithPosition(int rowIndex, int colIndex, int rowOffset, int colOffset) const
{
	auto newRowIndex = rowIndex + rowOffset;
	auto newColIndex = colIndex + colOffset;
	if (!isRowIndexValid(newRowIndex) || !isColIndexValid(newColIndex))
		return nullptr;

	return m_StarScripts[newRowIndex][newColIndex];
}

void StarMatrixScript::StarMatrixScriptImpl::explode(StarScript* star)
{
	if (!star)
		return;

	auto particleActor = SingletonContainer::getInstance()->get<GameLogic>()->createActor("Actors\\StarParticle.xml");
	particleActor->getComponent<StarParticleScript>()->show(star->getPositionX(), star->getPositionY(), star->getColor4F());
	m_Actor.lock()->addChild(*particleActor);

	m_StarScripts[star->getRowIndex()][star->getColIndex()] = nullptr;
	star->setVisible(false);
}

void StarMatrixScript::StarMatrixScriptImpl::removeEmptyBlocks()
{
	//Remove empty blocks for every column.
	for (auto colIndex = 0; colIndex < s_ColumnCount; ++colIndex){
		auto rowIndexOffset = 1;
		for (auto rowIndex = 0; rowIndex < s_RowCount; ++rowIndex){
			if (m_StarScripts[rowIndex][colIndex])
				continue;

			for (auto rowIndexForSwapping = rowIndex + rowIndexOffset; rowIndexForSwapping < s_RowCount; ++rowIndexForSwapping){
				if (m_StarScripts[rowIndexForSwapping][colIndex]){
					rowIndexOffset = rowIndexForSwapping - rowIndex;
					std::swap(m_StarScripts[rowIndex][colIndex], m_StarScripts[rowIndexForSwapping][colIndex]);
					break;
				}
			}
		}
	}

	//Remove empty columns.
	auto colIndexOffset = 1;
	for (auto colIndex = 0; colIndex < s_ColumnCount; ++colIndex){
		if (m_StarScripts[0][colIndex])
			continue;

		for (auto colIndexForSwapping = colIndex + colIndexOffset; colIndexForSwapping < s_ColumnCount; ++colIndexForSwapping){
			if (m_StarScripts[0][colIndexForSwapping]){
				colIndexOffset = colIndexForSwapping - colIndex;

				for (auto rowIndex = 0; rowIndex < s_RowCount && m_StarScripts[rowIndex][colIndexForSwapping]; ++rowIndex)
					std::swap(m_StarScripts[rowIndex][colIndex], m_StarScripts[rowIndex][colIndexForSwapping]);

				break;
			}
		}
	}

	//Visually move the stars to their new positions.
	for (auto rowIndex = 0; rowIndex < s_RowCount; ++rowIndex){
		for (auto colIndex = 0; colIndex < s_ColumnCount; ++colIndex){
			auto star = m_StarScripts[rowIndex][colIndex];

			if (star && (star->getRowIndex() != rowIndex || star->getColIndex() != colIndex)){
				star->setRowNum(rowIndex);
				star->setColNum(colIndex);

				auto position = getStarDefaultPosition(rowIndex, colIndex);
				star->moveTo(position.x, position.y);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation of StarMatrixScript.
//////////////////////////////////////////////////////////////////////////
StarMatrixScript::StarMatrixScript() : pimpl{ std::make_shared<StarMatrixScriptImpl>() }
{
}

StarMatrixScript::~StarMatrixScript()
{
}

bool StarMatrixScript::vInit(tinyxml2::XMLElement *xmlElement)
{
	static auto isStaticInitialized = false;
	if (isStaticInitialized)
		return true;

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

	//Read other data.
	pimpl->s_IntervalForExplodingRemainingStars = xmlElement->FirstChildElement("IntervalForExplodingRemainingStars")->FloatAttribute("Value");

	isStaticInitialized = true;
	return true;
}

void StarMatrixScript::vPostInit()
{
	pimpl->m_Actor = m_Actor;

	auto actor = m_Actor.lock();
	actor->getComponent<FiniteTimeActionComponent>()->setRunAutomatically(true);

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
			actor->addChild(*starActor);
		}

	auto eventDispatcher = SingletonContainer::getInstance()->get<IEventDispatcher>();
	eventDispatcher->vAddListener(EventType::LevelSummaryFinished, pimpl, [this](const IEventData & e){
		pimpl->onLevelSummaryFinished(e);
	});
	eventDispatcher->vAddListener(EventType::StartLevelLabelDisappeared, pimpl, [this](const IEventData & e){
		pimpl->onStartLevelLabelDisappeared(e);
	});
}

const std::string & StarMatrixScript::getType() const
{
	return Type;
}

const std::string StarMatrixScript::Type = "StarMatrixScript";
