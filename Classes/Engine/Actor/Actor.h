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
#include "../../Common/IUpdateable.h"

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
 * \
 * \author	Babygogogo
 * \date	2015.03
 */
class Actor final
{
	friend class Timer;
	friend class ActorFactory;
	
public:
	~Actor();

	//////////////////////////////////////////////////////////////////////////
	//Factory method.
	//////////////////////////////////////////////////////////////////////////
	//	You can supply the additional_task with a lambda, making the caller code more compact:
	//auto game_object = Actor::create([](Actor *obj){obj->addComponent<...>();...;});
	//	otherwise (the code below does the same thing as above):
	//auto game_object = Actor::create();
	//game_object->addComponent<...>();...;
	//	No matter which approach you prefer, make sure that your code is readable.
	static std::unique_ptr<Actor> create(std::string name, std::function<void(Actor*)> &&additional_task = nullptr)
	{
		auto game_object = std::unique_ptr<Actor>(new Actor(std::move(name)));
		if (additional_task)
			additional_task(game_object.get());

		return game_object;
	}

	//	You can also supply a concrete Component type as the template argument:
	//auto game_object = Actor::create<...>();
	//The according component will be added to the game object.
	template <typename Component_,
		typename std::enable_if_t<std::is_base_of<ActorComponent, Component_>::value>* = nullptr
	>
	static std::unique_ptr<Actor> create(std::string name, std::function<void(Actor*)> &&additional_task = nullptr)
	{
		auto game_object = Actor::create(std::move(name), [](Actor *game_object){game_object->addComponent<Component_>(); });
		if (additional_task)
			additional_task(game_object.get());

		return game_object;
	}

	static std::shared_ptr<Actor> create();

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

	void setNeedUpdate(bool is_need);

	//Disable copy/move constructor and operator=.
	Actor(const Actor&) = delete;
	Actor(Actor&&) = delete;
	Actor& operator=(const Actor&) = delete;
	Actor& operator=(Actor&&) = delete;

private:
	//Constructor is private because game objects are managed using std::unique_ptr and I provide a factory method for that.
	Actor();
	Actor(std::string && name);

	//Only the current scene owned by the SceneStack, and the (indirect) children of that scene, will be "update" once a frame.
	void update(const time_t &time_ms);

	//////////////////////////////////////////////////////////////////////////
	//Helper methods for adding components.
	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	T* emplaceComponent()
	{
		return dynamic_cast<T*>(m_components.emplace(typeid(T), std::unique_ptr<T>(new T(this))).first->second.get());
	}

	template<typename T,
		typename std::enable_if_t<!std::is_base_of<IUpdateable, T>::value>* = nullptr
	> T* addComponentHelper()
	{
		return emplaceComponent<T>();
	};

	template<typename T,
		typename std::enable_if_t<std::is_base_of<IUpdateable, T>::value>* = nullptr
	> T* addComponentHelper()
	{
		auto component = emplaceComponent<T>();
		m_updateable_components.emplace(component);

		return component;
	};

	//////////////////////////////////////////////////////////////////////////
	//Data members of components and children.
	//////////////////////////////////////////////////////////////////////////
	std::unordered_map<std::type_index, std::unique_ptr<::ActorComponent>> m_components;
	std::unordered_set<IUpdateable*> m_updateable_components;

	struct ActorImpl;
	std::unique_ptr<ActorImpl> pimpl;
};

#endif // !__ACTOR__
