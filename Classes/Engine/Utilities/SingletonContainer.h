#ifndef __SINGLETON_CONTAINER__
#define __SINGLETON_CONTAINER__

#include <memory>
#include <typeindex>

/*!
 * \brief Container of the singleton objects of the game. 
 *
 * \details
 *	Singleton objects can be added dynamically.
 *
 * \author Babygogogo
 * \date 2015.3
 */
class SingletonContainer final
{
public:
	~SingletonContainer();

	//get the one and only instance (creates the instance on the first call)
	static const std::unique_ptr<SingletonContainer> & getInstance();

	//Get an object of a given type. nullptr is returned if the object doesn't exist.
	template <typename T>
	std::shared_ptr<T> get() const{
		return std::static_pointer_cast<T>(getHelper(typeid(T)));
	}

	//Set an object of a given type (will replace the old one of the same type if exists).
	template <typename BasePointer, typename Derived,
		typename std::enable_if_t<std::is_base_of<BasePointer, Derived>::value>* = nullptr>
	std::shared_ptr<BasePointer> set(std::shared_ptr<Derived> && derived){
		return std::static_pointer_cast<BasePointer>(setHelper(typeid(BasePointer), std::move(derived)));
	}

	template <typename BasePointer, typename Derived,
		typename std::enable_if_t<std::is_base_of<BasePointer, Derived>::value>* = nullptr>
	std::shared_ptr<BasePointer> set(std::unique_ptr<Derived> && derived){
		return std::static_pointer_cast<BasePointer>(setHelper(typeid(BasePointer), std::move(derived)));
	}

	//delete copy/move constructor and operator=.
	SingletonContainer(const SingletonContainer&) = delete;
	SingletonContainer(SingletonContainer&&) = delete;
	SingletonContainer& operator=(const SingletonContainer&) = delete;
	SingletonContainer& operator=(SingletonContainer&&) = delete;

private:
	SingletonContainer();

	//Non-template helper functions for set/get object.
	std::shared_ptr<void> getHelper(const std::type_index & typeIndex) const;
	std::shared_ptr<void> setHelper(std::type_index && typeIndex, std::shared_ptr<void> && obj);

	//The one and only instance.
	static std::unique_ptr<SingletonContainer> s_Instance;

	//The implementation stuff.
	struct SingletonContainerImpl;
	std::unique_ptr<SingletonContainerImpl> pimpl;
};

#endif // !__SINGLETON_CONTAINER__
