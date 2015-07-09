#include <functional>
#include <cassert>
#include <unordered_map>

#include "GenericFactory.h"
#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
//Definition of GenericFactory<Base>::GenericFactoryImpl
//////////////////////////////////////////////////////////////////////////
//template <typename Base>
//struct GenericFactory<Base>::GenericFactoryImpl
//{
//	GenericFactoryImpl(){};
//	~GenericFactoryImpl(){};
//
//	std::unordered_map<std::string, std::function<std::unique_ptr<Base>>> m_CreationFunctions;
//};
//
//template <typename Base>
//GenericFactory<Base>::GenericFactoryImpl::GenericFactoryImpl()
//{
//
//}
//
//template <typename Base>
//GenericFactory<Base>::GenericFactoryImpl::~GenericFactoryImpl()
//{
//
//}

//////////////////////////////////////////////////////////////////////////
//Implementation of GenericFactory.
//////////////////////////////////////////////////////////////////////////
template <class Base>
GenericFactory<Base>::GenericFactory() //:pimpl(new GenericFactory<Base>::GenericFactoryImpl())
{

}

template <class Base>
GenericFactory<Base>::~GenericFactory()
{

}

template <class Base>
std::unique_ptr<GenericFactory<Base>> GenericFactory<Base>::createFactory()
{
	return std::unique_ptr<GenericFactory<Base>>(new GenericFactory<Base>());
}

//template <class Base>
//std::unique_ptr<Base> GenericFactory<Base>::createObject(const std::string & name)
//{
//	auto findIter = pimpl->m_creationFunctions.find(name);
//	if (findIter != pimpl->m_creationFunctions.end())
//		return findIter->second();
//
//	return nullptr;
//}

//template <class Base>
//void GenericFactory<Base>::registerHelper(std::string && name, std::function<std::unique_ptr<Base>> && creationFunction)
//{
//	if (pimpl->m_CreationFunctions.find(name) != pimpl->m_CreationFunctions.end()){
//		cocos2d::log("GenericFactory::registerHelper overwriting an existing creation function.");
//		pimpl->m_CreationFunctions[name] = std::move(creationFunction);
//	}
//	else
//		pimpl->m_CreationFunctions.emplace(std::make_pair(std::move(name), std::move(creationFunction)));
//}
