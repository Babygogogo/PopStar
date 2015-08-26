#include <map>
#include <list>
#include <cassert>
#include <mutex>
#include <functional>

#include "cocos2d.h"

#include "GameLogic.h"
#include "../Actor/Actor.h"
#include "../Actor/ActorFactory.h"
#include "../Event/EventType.h"
#include "../Event/EvtDataRequestDestroyActor.h"
#include "../Event/IEventDispatcher.h"
#include "../MainLoop/ProcessRunner.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameLogicImpl.
//////////////////////////////////////////////////////////////////////////
struct GameLogic::GameLogicImpl
{
public:
	GameLogicImpl();
	~GameLogicImpl();

	void onRequestDestroyActor(const IEventData & e);

	ProcessRunner m_ProcessRunner;

	//Lock of the process of updating actor.
	bool m_IsUpdatingActors{ false };

	std::map<ActorID, std::shared_ptr<Actor>> m_Actors;
	std::unique_ptr<ActorFactory> m_ActorFactory{ std::make_unique<ActorFactory>() };

	//The operations that are blocked because of the lock. They should be executed when the lock is unlocked.
	std::list<std::function<void()>> m_CachedOperations;
};

GameLogic::GameLogicImpl::GameLogicImpl()
{
}

GameLogic::GameLogicImpl::~GameLogicImpl()
{
}

void GameLogic::GameLogicImpl::onRequestDestroyActor(const IEventData & e)
{
	auto actorID = (static_cast<const EvtDataRequestDestoryActor &>(e)).getActorID();

	if (m_IsUpdatingActors)
		m_CachedOperations.emplace_back([actorID, this](){m_Actors.erase(actorID); });
	else
		m_Actors.erase(actorID);
}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameLogic.
//////////////////////////////////////////////////////////////////////////
GameLogic::GameLogic() : pimpl{ std::make_shared<GameLogicImpl>() }
{
	static int InstanceCount{ 0 };
	assert((InstanceCount++ == 0) && "GameLogic is created more than once!");

	SingletonContainer::getInstance()->get<IEventDispatcher>()->vAddListener(EventType::RequestDestoryActor, pimpl, [this](const IEventData & e){pimpl->onRequestDestroyActor(e); });
}

GameLogic::~GameLogic()
{
}

void GameLogic::vUpdate(const std::chrono::milliseconds & deltaTimeMs)
{
	//Update the process runner.
	pimpl->m_ProcessRunner.updateAllProcess(deltaTimeMs);

	//Lock the lock of updating actors.
	pimpl->m_IsUpdatingActors = true;

	//Update all of the actors.
	for (auto & idActorPair : pimpl->m_Actors)
		idActorPair.second->update(deltaTimeMs);

	//Unlock the lock of updating actors.
	pimpl->m_IsUpdatingActors = false;

	//Execute the operations that are blocked by the updating process.
	while (!pimpl->m_CachedOperations.empty()){
		pimpl->m_CachedOperations.front()();
		pimpl->m_CachedOperations.pop_front();
	}
}

const std::shared_ptr<Actor> & GameLogic::getActor(const ActorID & id) const
{
	//Try to find the actor corresponding to the id.
	auto actorIter = pimpl->m_Actors.find(id);

	//If fail to find the actor, return nullptr.
	if (actorIter == pimpl->m_Actors.end()){
		static const auto nullActor = std::shared_ptr<Actor>(nullptr);
		return nullActor;
	}

	//Succeed to find the actor and return it.
	return actorIter->second;
}

std::shared_ptr<Actor> GameLogic::createActor(const char *resourceFile, tinyxml2::XMLElement *overrides /*= nullptr*/)
{
	//Try to create the actor.
	auto newActor = pimpl->m_ActorFactory->createActor(resourceFile, overrides);

	//Failed to create the actor; return nullptr.
	if (!newActor){
		static const auto nullActor = std::shared_ptr<Actor>(nullptr);
		return nullActor;
	}

	//Succeed to create the actor. Add it to actor map and return it.
	//std::map::emplace doesn't invalidate any iterators so it's safe to ignore the lock.
	auto emplaceResult = pimpl->m_Actors.emplace(std::make_pair(newActor->getID(), std::move(newActor)));
	assert(emplaceResult.second);
	return emplaceResult.first->second;
}

void GameLogic::destroyActor(const ActorID & id)
{
	//Try to find the actor corresponding to the id.
	auto actorIter = pimpl->m_Actors.find(id);

	//If the id is not in use, simply return.
	if (actorIter == pimpl->m_Actors.end())
		return;

	//It's not safe to remove actor from the map when updating actors.
	//If it's the case, cache the operation and execute it after updating.
	if (pimpl->m_IsUpdatingActors){
		pimpl->m_CachedOperations.emplace_back([this, id](){destroyActor(id); });
		return;
	}

	//It's not updating actors; just remove the actor.
	pimpl->m_Actors.erase(actorIter);
}
