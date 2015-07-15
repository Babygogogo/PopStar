#ifndef __GAME_LOGIC__
#define __GAME_LOGIC__

#include <memory>
#include <chrono>

#include "../Actor/ActorID.h"
#include "../../cocos2d/external/tinyxml2/tinyxml2.h"

class Actor;

/*!
 * \class GameLogic
 *
 * \brief Represents the world of the game.
 *
 * \details
 *	This class should be singleton.
 *	By now, its main job is to maintain a list of all the actors in the game world.
 *	Avoid owning any std::shared_ptr<Actor> outside this class, otherwise it will be hard to destory that actor.
 *	Much more work to do on this class...
 *
 * \author Babygogogo
 * \date 2015.7
 */
class GameLogic
{
public:
	~GameLogic();

	//Create an instance of this class. Only the first call is valid.
	static std::unique_ptr<GameLogic> create();

	//Update all actors (and views and other things later...)
	void update(const std::chrono::milliseconds & deltaTimeMs);

	//Get actor with an id. You will get nullptr if the id is not in use.
	const std::shared_ptr<Actor> & getActor(const ActorID & id) const;

	//Create an actor with a .xml file. The actor will be add into the internal actor list.
	const std::shared_ptr<Actor> & createActor(const char *resourceFile, tinyxml2::XMLElement *overrides = nullptr);

	//Remove an actor with an id from the internal actor list. The actor will be destroyed if you don't own it elsewhere.
	//Nothing happens if the id is not in use.
	//Warning: If you own a std::shared_ptr<Actor> corresponding to the id outside this class, the destruction is not guaranteed to happen.
	void destroyActor(const ActorID & id);

	//Disable copy/move constructor and operator=.
	GameLogic(const GameLogic&) = delete;
	GameLogic(GameLogic&&) = delete;
	GameLogic& operator=(const GameLogic&) = delete;
	GameLogic& operator=(GameLogic&&) = delete;
	
private:
	GameLogic();

	//Implementation stuff.
	struct GameLogicImpl;
	std::unique_ptr<GameLogicImpl> pimpl;
};

#endif // !__GAME_LOGIC__
