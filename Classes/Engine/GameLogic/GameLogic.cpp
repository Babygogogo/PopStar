#include <map>
#include <list>
#include <cassert>
#include <mutex>
#include <functional>

#include "GameLogic.h"
#include "../Actor/Actor.h"
#include "../Actor/ActorFactory.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GameLogicImpl.
//////////////////////////////////////////////////////////////////////////
struct GameLogic::GameLogicImpl
{
public:
	GameLogicImpl();
	~GameLogicImpl();

	//Lock of the process of updating actor.
	bool m_IsUpdatingActors{ false };

	std::map<ActorID, std::shared_ptr<Actor>> m_Actors;
	std::unique_ptr<ActorFactory> m_ActorFactory;

	//The operations that are blocked because of the lock. They should be executed when the lock is unlocked.
	std::list<std::function<void()>> m_CachedOperations;
};

GameLogic::GameLogicImpl::GameLogicImpl() : m_ActorFactory(ActorFactory::createFactory())
{

}

GameLogic::GameLogicImpl::~GameLogicImpl()
{

}

//////////////////////////////////////////////////////////////////////////
//Implementation of GameLogic.
//////////////////////////////////////////////////////////////////////////
GameLogic::GameLogic() : pimpl(new GameLogicImpl())
{

}

GameLogic::~GameLogic()
{

}

std::unique_ptr<GameLogic> GameLogic::create()
{
	auto newGameLogic = std::unique_ptr<GameLogic>(nullptr);

	//Ensures that this function only creates the instance once.
	static std::once_flag creationFlag;
	std::call_once(creationFlag, [&newGameLogic](){newGameLogic.reset(new GameLogic()); });

	return newGameLogic;
}

void GameLogic::update(const std::chrono::milliseconds & deltaTimeMs)
{
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

const std::shared_ptr<Actor> & GameLogic::createActor(const char *resourceFile, tinyxml2::XMLElement *overrides /*= nullptr*/)
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
