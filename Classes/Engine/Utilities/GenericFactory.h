#ifndef __GENERIC_FACTORY__
#define __GENERIC_FACTORY__

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <cassert>

/*!
 * \class GenericFactory<BasePointer>
 *
 * \brief A generic factory that allows to create objects of sub-type of Base by strings.
 *
 * \details
 *	The template argument Base is the type of pointer you get when you call createXXX(). You can downcast the pointer to a concrete one.
 *	You must call registerType() before you can create that object of that type using this factory. The type must be a sub-type of Base.
 *	This class should be used inside concrete factories. That is, avoid #include this file in other headers.
 *
 * \author Babygogogo
 * \date 2015.7
 */
template <class Base>
class GenericFactory
{
public:
	GenericFactory() = default;
	~GenericFactory() = default;

	//Register a type. The type must be a sub-type of Base.
	//If you double register a type, an assertion will be triggered.
	//The SubClass must have a static member named Type, which is the type name of the SubClass.
	template <class SubClass>
	void registerType()
	{
		assert(m_CreationFunctions.find(SubClass::Type) == m_CreationFunctions.end());

		CreatorFunction creator;
		creator.createUnique = std::make_unique < SubClass > ;
		creator.createShared = std::make_shared < SubClass > ;

		m_CreationFunctions.emplace(std::make_pair(SubClass::Type, std::move(creator)));
	}

	//Create an object. This function return the pointer of the Base class which can be downcast by client code.
	//If no type is registered with the name, nullptr is returned.
	std::unique_ptr<Base> createUnique(const std::string & name)
	{
		auto findIter = m_CreationFunctions.find(name);
		if (findIter != m_CreationFunctions.end())
			return findIter->second.createUnique();

		return nullptr;
	}

	std::shared_ptr<Base> createShared(const std::string & name)
	{
		auto findIter = m_CreationFunctions.find(name);
		if (findIter != m_CreationFunctions.end())
			return findIter->second.createShared();

		return nullptr;
	}

private:
	struct CreatorFunction
	{
		std::function < std::unique_ptr<Base>() > createUnique;
		std::function < std::shared_ptr<Base>() > createShared;
	};

	std::unordered_map<std::string, CreatorFunction> m_CreationFunctions;
};

#endif // !__GENERIC_FACTORY__
