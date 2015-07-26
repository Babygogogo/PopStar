#ifndef __ACTOR__
#define __ACTOR__

#include <memory>
#include <chrono>
#include <string>

#include "ActorID.h"

//Forward declaration.
namespace tinyxml2
{
	class XMLElement;
}
class ActorComponent;

/*!
 * \brief	Almost everything in the game is a Actor, such as an unit, a strategy map, an effect of an explosion, and so on.
 * \
 * \details
 *	Actors are organized in the form of trees. Methods like addChild, removeFromParent are provided for organizing.
 *	Roots of the trees are Actors that created as "scene". They should be push into SceneStack to take their effects.
 *	Game object is a container of various components and/or scripts which implement most of the logics of the real game object.
 *	Game objects, along with its components, are managed with std::unique_ptr.
 *	For most cases in the code base, a std::unique_ptr means an ownership, while a raw pointer stands for a (temporary) reference.
 *	Now refactoring...
 * \
 * \author	Babygogogo
 * \date	2015.03
 */
class Actor final
{
	friend class Timer;
	friend class ActorFactory;

public:
	Actor();
	~Actor();

	//Called by ActorFactory right after the actor is created.
	//Loads the basic data of the actor from the xmlElement. Doesn't create or attach any component.
	bool init(ActorID id, tinyxml2::XMLElement *xmlElement);

	//Called by ActorFactory after the actor is initialized and all its components are attached.
	//Calls vPostInit() on every attached component.
	void postInit();

	//Called by GameLogic on every game loop.
	//Calles vUpdate() on every attached component.
	void update(const std::chrono::milliseconds & delteTimeMs);

	const ActorID & getID() const;

	//Stuff for adding/getting components or scripts.
	//Called by ActorFactory during the creation of the actor.
	void addComponent(std::shared_ptr<ActorComponent> && component);

	//The convenient function for adding component manually.
	//#TODO: This should be removed when the refactoring is done.
	template<typename T,
		typename std::enable_if_t<std::is_base_of<ActorComponent, T>::value>* = nullptr
	> std::shared_ptr<T> addComponent()	//T must derive from Component
	{
		if (auto existingComponent = getComponent<T>())
			return existingComponent;

		addComponent(std::make_shared<T>(this));
		return getComponent<T>();
	}

	//Get an attached component by its type name. Returns nullptr if no such component attached.
	//Warning: You should not own the shared_ptr returned by this method. Instead, own weak_ptr.
	std::shared_ptr<ActorComponent> getComponent(const std::string & type) const;

	//The convenient function for getting component, which automatically downcast the pointer.
	//Returns nullptr if no such component attached.
	//Warning: You should not own the shared_ptr returned by this method. Instead, own weak_ptr.
	template<typename T>
	std::shared_ptr<T> getComponent() const	//T should derive from Component
	{
		return std::static_pointer_cast<T>(getComponent(T::Type));
	}

	//Stuff for organizing the Actors as trees.
	//#TODO: Remove these functions after the refactoring is done because actors need not to be in the form of trees.
	std::weak_ptr<Actor> addChild(std::shared_ptr<Actor> && child);
	Actor *getParent() const;
	bool isAncestorOf(const Actor *child) const;
	//If the game object has no parent, nothing happens, and nullptr is returned.
	//Otherwise, the ownership is returned. Keep it, or the actor along with its children will be destroyed.
	//	std::unique_ptr<Actor> removeFromParent();
	std::shared_ptr<Actor> removeFromParent();

	//Disable copy/move constructor and operator=.
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	//Only the current scene owned by the SceneStack, and the (indirect) children of that scene, will be "update" once a frame.
	void update(const time_t &time_ms);

	//Implementation stuff.
	struct ActorImpl;
	std::unique_ptr<ActorImpl> pimpl;
};

#endif // !__ACTOR__
