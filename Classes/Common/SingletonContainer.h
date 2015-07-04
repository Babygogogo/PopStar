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

	//get the one and only instance (will create the instance if necessary)
	static const std::unique_ptr<SingletonContainer> & getInstance();

	//method templates for get/set object
	template <typename T>
	std::shared_ptr<T> get() const{
		return std::static_pointer_cast<T>(getHelper(typeid(T)));
	}

	template <typename T>
	std::shared_ptr<T> set(){
		return std::static_pointer_cast<T>(setHelper(typeid(T), T::create()));
	}

	//delete copy/move constructor and operator=.
	SingletonContainer(const SingletonContainer&) = delete;
	SingletonContainer(SingletonContainer&&) = delete;
	SingletonContainer& operator=(const SingletonContainer&) = delete;
	SingletonContainer& operator=(SingletonContainer&&) = delete;

private:
	SingletonContainer();

	//non-template helper functions for set/get object
	std::shared_ptr<void> getHelper(const std::type_index & typeIndex) const;
	std::shared_ptr<void> setHelper(std::type_index && typeIndex, std::shared_ptr<void> && obj);

	//the one and only instance
	static std::unique_ptr<SingletonContainer> s_Instance;

	//the implementation stuff
	struct SingletonContainerImpl;
	std::unique_ptr<SingletonContainerImpl> pimpl;
};

#endif // !__SINGLETON_CONTAINER__
