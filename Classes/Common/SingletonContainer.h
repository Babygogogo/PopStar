#ifndef __BW_SINGLETONS__
#define __BW_SINGLETONS__

#include <memory>
#include <mutex>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include "Object.h"

/*!
 * \brief Container of the singleton objects of the game. 
 *
 * \details
 *	Singleton objects can be added dynamically.
 *
 * \author Babygogogo
 * \date 2015.3
 */
class SingletonContainer final : public Object
{
public:
	virtual ~SingletonContainer();

	static SingletonContainer &instance();

	template <typename T>
	T* add(){
		if (typeid(T) == typeid(SingletonContainer))
			throw("adding BWSingletons to BWSingletons.");

		if (auto existing_composite = get<T>())
			return existing_composite;

		return dynamic_cast<T*>(m_composites.emplace(typeid(T), std::unique_ptr<T>(new T(this))).first->second.get());
	}

	template <typename T>
	T* get() const{
		auto composite_iter = m_composites.find(typeid(T));
		return composite_iter == m_composites.end() ?
			nullptr :
			dynamic_cast<T*>(composite_iter->second.get());
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

	static std::once_flag m_init_flag;
	static std::unique_ptr<SingletonContainer> m_instance;

	std::unordered_map<std::type_index, std::unique_ptr<Object>> m_composites;
};

#endif // !__BW_SINGLETONS__
