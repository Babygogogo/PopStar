#ifndef __GENERIC_FACTORY__
#define __GENERIC_FACTORY__

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "cocos2d.h"

/*!
 * \class GenericFactory<BasePointer>
 *
 * \brief A generic factory that allows to create objects of sub-type of Base by strings.
 *
 * \details
 *	The template argument (BasePointer) is the type of pointer you get when you call createObject(). You can downcast the pointer to a concrete one.
 *	You must call registerType() before you can create that object of that type using this factory. The type must be a sub-type of Base.
 *	This class should be used inside concrete factories. That is, avoid #include this file in other headers.
 *
 * \author Babygogogo
 * \date 2015.7
 */
template <class BasePointer>
class GenericFactory
{
public:
	~GenericFactory(){};

	//Create the factory itself.
	static std::unique_ptr<GenericFactory> createFactory()
	{
		return std::unique_ptr<GenericFactory>(new GenericFactory());
	};

	//Register a type. The type must be a sub-type of Base.
	//If you double register a type, the first registration will be overwritten.
	template <class SubClass>
	void registerType(const std::string & name)
	{
		if (m_CreationFunctions.find(name) != m_CreationFunctions.end()){
			cocos2d::log("GenericFactory::registerObject overwriting an existing creation function.");
			m_CreationFunctions[name] = [](){return SubClass::create(); };
		}
		else
			m_CreationFunctions.emplace(std::make_pair(name, [](){return SubClass::create(); }));
	}

	template <class SubClass>
	void registerType()
	{
		registerType<SubClass>(SubClass::Type);
	}

	//Create an object. This function return the pointer of the Base class which can be downcast by client code.
	//If no type is registered with the name, nullptr is returned.
	BasePointer createObject(const std::string & name)
	{
		auto findIter = m_CreationFunctions.find(name);
		if (findIter != m_CreationFunctions.end())
			return findIter->second();

		return nullptr;
	};

private:
	GenericFactory(){};

	std::unordered_map<std::string, std::function<BasePointer()>> m_CreationFunctions;
};

#endif // !__GENERIC_FACTORY__
