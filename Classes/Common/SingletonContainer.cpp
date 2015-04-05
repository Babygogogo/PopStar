#include "SingletonContainer.h"

std::once_flag SingletonContainer::m_init_flag;
std::unique_ptr<SingletonContainer> SingletonContainer::m_instance{ nullptr };

SingletonContainer::SingletonContainer() :Object("SingletonContainer")
{

}

SingletonContainer::~SingletonContainer()
{

}

SingletonContainer & SingletonContainer::instance()
{
	if (!m_instance)
		std::call_once(m_init_flag, []{m_instance.reset(new SingletonContainer); });

	return *m_instance;
}
