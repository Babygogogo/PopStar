#include <vector>
#include <cassert>

#include "cocos2d.h"

#include "SceneStack.h"
#include "../Actor/Actor.h"
#include "../Actor/GeneralRenderComponent.h"
#include "../Utilities/SingletonContainer.h"

//////////////////////////////////////////////////////////////////////////
//Definition of SceneStackImpl.
//////////////////////////////////////////////////////////////////////////
struct SceneStack::SceneStackImpl
{
	SceneStackImpl();
	~SceneStackImpl();

	void validate(Actor* scene);

	void push(std::shared_ptr<Actor> &&scene);
	std::shared_ptr<Actor> pop();

	void pushSceneToDirector(Actor *scene);
	Actor *topScene();

	std::vector<std::shared_ptr<Actor>> m_scenes;
};

SceneStack::SceneStackImpl::SceneStackImpl()
{
}

SceneStack::SceneStackImpl::~SceneStackImpl()
{
}

void SceneStack::SceneStackImpl::pushSceneToDirector(Actor *scene)
{
	auto director = cocos2d::Director::getInstance();
	if (director->getRunningScene())
		director->pushScene(scene->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Scene>());
	else
		director->runWithScene(scene->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Scene>());
}

Actor * SceneStack::SceneStackImpl::topScene()
{
	if (m_scenes.empty())
		throw ("Getting topScene while there are no scenes.");

	return m_scenes.back().get();
}

void SceneStack::SceneStackImpl::validate(Actor* scene)
{
	if (!scene || !scene->getComponent<GeneralRenderComponent>() || !scene->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Scene>())
		throw("pushScene with a non-scene");
	if (scene->hasParent())
		throw("pushScene with a game object that has a parent");
}

void SceneStack::SceneStackImpl::push(std::shared_ptr<Actor> &&scene)
{
	m_scenes.emplace_back(std::move(scene));
}

std::shared_ptr<Actor> SceneStack::SceneStackImpl::pop()
{
	auto ownership = std::move(m_scenes.back());
	m_scenes.pop_back();

	return ownership;
}

//////////////////////////////////////////////////////////////////////////
//Implementation of SceneStack.
//////////////////////////////////////////////////////////////////////////
SceneStack::SceneStack() : pimpl{ std::make_unique<SceneStackImpl>() }
{
	static int InstanceCount{ 0 };
	assert((InstanceCount++ == 0) && "SceneStack is created more than once!");
}

SceneStack::~SceneStack()
{
}

void SceneStack::pushAndRun(std::shared_ptr<Actor> && scene)
{
	pimpl->validate(scene.get());

	//Disable update for old top scene, then push the param scene into stack and enable it.
	pimpl->push(std::move(scene));

	pimpl->pushSceneToDirector(pimpl->topScene());
}

std::shared_ptr<Actor> SceneStack::pop()
{
	if (pimpl->m_scenes.size() <= 1)
		throw("popScene when the size of scenes <= 1");

	//Pop the current scene and enable the new top scene.
	auto ownership = pimpl->pop();

	cocos2d::Director::getInstance()->popScene();

	return ownership;
}

Actor* SceneStack::getCurrentScene()
{
	return pimpl->topScene();
}

std::shared_ptr<Actor> SceneStack::replaceAndRun(std::shared_ptr<Actor> &&scene)
{
	pimpl->validate(scene.get());
	if (pimpl->m_scenes.empty())
		throw("replaceAndRun scene when there are no scenes at all");

	auto ownership = pimpl->pop();
	pimpl->push(std::move(scene));

	cocos2d::Director::getInstance()->replaceScene(pimpl->topScene()->getComponent<GeneralRenderComponent>()->getAs<cocos2d::Scene>());

	return ownership;
}
