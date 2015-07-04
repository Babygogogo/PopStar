#ifndef __SINGLETON_CONTAINER__
#define __SINGLETON_CONTAINER__

#include <memory>
#include <unordered_map>
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

	static const std::unique_ptr<SingletonContainer> & getInstance();

	//////////////////////////////////////////////////////////////////////////
	//Methods for adding/getting composites
	//////////////////////////////////////////////////////////////////////////
	template <typename T>
	T* set(){
		if (auto existing_composite = get<T>())
			return existing_composite;

		return static_cast<T*>(m_Objects.emplace(typeid(T), T::create()).first->second.get());
	}

	template <typename T>
	T* get() const{
		auto composite_iter = m_Objects.find(typeid(T));
		return composite_iter == m_Objects.end() ?
			nullptr :
			static_cast<T*>(composite_iter->second.get());
	}

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	SingletonContainer(const SingletonContainer&) = delete;
	SingletonContainer(SingletonContainer&&) = delete;
	SingletonContainer& operator=(const SingletonContainer&) = delete;
	SingletonContainer& operator=(SingletonContainer&&) = delete;

private:
	SingletonContainer();

	//helper function for set/get object
	const std::shared_ptr<void> & getHelper(const std::type_index & typeIndex) const;
	const std::shared_ptr<void> & setHelper(std::type_index && typeIndex, std::shared_ptr<void> && obj);

	//the only and one instance
	static std::unique_ptr<SingletonContainer> s_Instance;

	struct SingletonContainerImpl;
	std::unique_ptr<SingletonContainerImpl> pimpl;

	std::unordered_map<std::type_index, std::shared_ptr<void>> m_Objects;
};

#endif // !__SINGLETON_CONTAINER__
