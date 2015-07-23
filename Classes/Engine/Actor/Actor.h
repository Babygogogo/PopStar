#ifndef __ACTOR__
#define __ACTOR__

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <type_traits>
#include <functional>
#include <chrono>

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

	bool init(ActorID id, tinyxml2::XMLElement *xmlElement);
	void postInit();
	void update(const std::chrono::milliseconds & delteTimeMs);

	void addComponent(std::shared_ptr<ActorComponent> && component);
	const ActorID & getID() const;
	std::shared_ptr<ActorComponent> getComponent(const std::string & type) const;

	//////////////////////////////////////////////////////////////////////////
	//Stuff for organizing the Actors as trees.
	//////////////////////////////////////////////////////////////////////////
	//	Actor *addChild(std::unique_ptr<Actor>&& child);
	std::weak_ptr<Actor> addChild(std::shared_ptr<Actor> && child);
	Actor *getParent() const;
	bool isAncestorOf(const Actor *child) const;

	//If the game object has no parent, nothing happens, and nullptr is returned.
	//Otherwise, the ownership is returned. Keep it, or the object along with its children will be destroyed.
	//	std::unique_ptr<Actor> removeFromParent();
	std::shared_ptr<Actor> removeFromParent();

	//////////////////////////////////////////////////////////////////////////
	//Stuff for adding/getting components or scripts.
	//////////////////////////////////////////////////////////////////////////
	template<typename T,
		typename std::enable_if_t<std::is_base_of<ActorComponent, T>::value>* = nullptr
	> T* addComponent()	//T must derive from Component
	{
		if (auto existing_component = getComponent<T>())
			return existing_component;

		return addComponentHelper<T>();
	};

	template<typename T>
	T* getComponent() const	//T should derive from Component
	{
		auto component_iter = m_components.find(typeid(T));
		return component_iter == m_components.end() ?
			nullptr :
			dynamic_cast<T*>(component_iter->second.get());
	};

	//Disable copy/move constructor and operator=.
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	//Only the current scene owned by the SceneStack, and the (indirect) children of that scene, will be "update" once a frame.
	void update(const time_t &time_ms);

	//Helper methods for adding components.
	template<typename T>
	T* addComponentHelper()
	{
		return dynamic_cast<T*>(m_components.emplace(typeid(T), std::unique_ptr<T>(new T(this))).first->second.get());
	};

	//Data members of components and children.
	std::unordered_map<std::type_index, std::unique_ptr<::ActorComponent>> m_components;

	//Implementation stuff.
	struct ActorImpl;
	std::unique_ptr<ActorImpl> pimpl;
};

#endif // !__ACTOR__
