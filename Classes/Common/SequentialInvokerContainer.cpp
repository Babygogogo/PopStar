#include "SequentialInvokerContainer.h"
#include "SequentialInvoker.h"

#include <unordered_map>
#include <list>

struct SequentialInvokerContainer::impl
{
	impl(){};
	~impl(){};

	void deleteUnusedInvokers();

	std::unordered_map<GameObject*, std::unique_ptr<SequentialInvoker>> m_invokers;
	std::list<std::unique_ptr<SequentialInvoker>> m_unused_invokers;
};

void SequentialInvokerContainer::impl::deleteUnusedInvokers()
{
	m_unused_invokers.remove_if([](const std::unique_ptr<SequentialInvoker> &p){return !p->isInvoking(); });
}

SequentialInvokerContainer::SequentialInvokerContainer() :Object("SequentialInvokerContainer"), pimpl(new impl)
{

}

SequentialInvokerContainer::~SequentialInvokerContainer()
{

}

std::unique_ptr<SequentialInvokerContainer> SequentialInvokerContainer::create()
{
	return std::unique_ptr<SequentialInvokerContainer>(new SequentialInvokerContainer);
}

SequentialInvoker * SequentialInvokerContainer::registerTarget(GameObject *game_object)
{
	pimpl->deleteUnusedInvokers();
	return pimpl->m_invokers.emplace(std::move(game_object), SequentialInvoker::create(game_object)).first->second.get();
}

SequentialInvoker * SequentialInvokerContainer::getInvokerBy(GameObject *game_object)
{
	pimpl->deleteUnusedInvokers();

	auto existing_iter = pimpl->m_invokers.find(game_object);
	if (existing_iter != pimpl->m_invokers.end())
		return existing_iter->second.get();

	return nullptr;
}

void SequentialInvokerContainer::deleteTarget(GameObject *game_object)
{
	pimpl->deleteUnusedInvokers();

	auto existing_iter = pimpl->m_invokers.find(game_object);
	if (existing_iter == pimpl->m_invokers.end())
		return;

	if (existing_iter->second->isInvoking()){
		pimpl->m_unused_invokers.push_back(std::unique_ptr<SequentialInvoker>(existing_iter->second.release()));
		pimpl->m_invokers.erase(existing_iter);
		return;
	}

	pimpl->m_invokers.erase(game_object);
}
