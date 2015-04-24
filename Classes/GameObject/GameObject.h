#ifndef __GAME_OBJECT__
#define __GAME_OBJECT__

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <typeindex>
#include <type_traits>
#include <functional>

#include "../Common/Object.h"
#include "../Common/IUpdateable.h"

class Component;

/*!
 * \brief	Almost everything in the game is a GameObject, such as an unit, a strategy map, an effect of an explosion, and so on.
 * \
 * \details
 *	GameObjects are organized in the form of trees. Methods like addChild, removeFromParent are provided for organizing.
 *	Roots of the trees are GameObjects that created as "scene". They should be push into SceneStack to take their effects.
 *	Game object is a container of various components and/or scripts which implement most of the logics of the real game object.
 *	Game objects, along with its components, are managed with std::unique_ptr.
 *	For most cases in the code base, a std::unique_ptr means an ownership, while a raw pointer stands for a (temporary) reference.
 * \
 * \author	Babygogogo
 * \date	2015.03
 */
class GameObject final : public Object
{
public:
	//////////////////////////////////////////////////////////////////////////
	//Factory method.
	//////////////////////////////////////////////////////////////////////////
	//	You can supply the additional_task with a lambda, making the caller code more compact:
	//auto game_object = GameObject::create([](GameObject *obj){obj->addComponent<...>();...;});
	//	otherwise (the code below does the same thing as above):
	//auto game_object = GameObject::create();
	//game_object->addComponent<...>();...;
	//	No matter which approach you prefer, make sure that your code is readable.
	static std::unique_ptr<GameObject> create(std::function<void(GameObject*)> &&additional_task = nullptr)
	{
		auto game_object = std::unique_ptr<GameObject>(new GameObject);
		if (additional_task)
			additional_task(game_object.get());

		return game_object;
	}

	//	You can also supply a concrete Component type as the template argument:
	//auto game_object = GameObject::create<...>();
	//The according component will be added to the game object.
	template <typename Component_,
		typename std::enable_if_t<std::is_base_of<Component, Component_>::value>* = nullptr
	>
	static std::unique_ptr<GameObject> create(std::function<void(GameObject*)> &&additional_task = nullptr)
	{
		auto game_object = GameObject::create([](GameObject *game_object){game_object->addComponent<Component_>(); });
		if (additional_task)
			additional_task(game_object.get());

		return game_object;
	}

	~GameObject();

	//////////////////////////////////////////////////////////////////////////
	//Stuff for organizing the GameObjects as trees.
	//////////////////////////////////////////////////////////////////////////
	void addChild(std::unique_ptr<GameObject>&& child);
	GameObject *getParent() const;
	bool isAncestorOf(const GameObject *child) const;

	//If the game object has no parent, nothing happens, and nullptr is returned.
	//Otherwise, the ownership is returned. Keep it, or the object along with its children will be destroyed.
	std::unique_ptr<GameObject> getOwnershipFromParent();

	//////////////////////////////////////////////////////////////////////////
	//Stuff for adding/getting components or scripts.
	//////////////////////////////////////////////////////////////////////////
	template<typename T,
		typename std::enable_if_t<std::is_base_of<Component, T>::value>* = nullptr
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

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	GameObject(const GameObject&) = delete;
	GameObject(GameObject&&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject& operator=(GameObject&&) = delete;

private:
	//Constructor is private because game objects are managed using std::unique_ptr and I provide a factory method for that.
	GameObject(const std::string &name = "");

	friend class Timer;
	//Only the current scene owned by the SceneStack, and the (indirect) children of that scene, will be "update" once a frame.
	void update(const time_t &time_ms)
	{
		for (auto &updateable : m_updateables)
			updateable->update(time_ms);

		for (auto &child : m_children)
			child->update(time_ms);
	};

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
		m_updateables.emplace(component);

		return component;
	};

	//////////////////////////////////////////////////////////////////////////
	//Data members of components and children.
	//////////////////////////////////////////////////////////////////////////
	std::unordered_map<std::type_index, std::unique_ptr<::Component>> m_components;
	std::unordered_set<IUpdateable*> m_updateables;
	std::vector<std::unique_ptr<GameObject>> m_children;

	class impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__GAME_OBJECT__
