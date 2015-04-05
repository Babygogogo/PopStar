#include <vector>
#include "SceneStack.h"
#include "SingletonContainer.h"
#include "Timer.h"
#include "../GameObject/DisplayNode.h"

struct SceneStack::impl
{
	impl();
	~impl();

	void switchUpdate(GameObject *scene, bool enable = true);
	void pushSceneToDirector(GameObject *scene);
	GameObject *topScene();

	std::vector<std::unique_ptr<GameObject>> m_scenes;
};

SceneStack::impl::impl()
{

}

SceneStack::impl::~impl()
{

}

void SceneStack::impl::switchUpdate(GameObject *scene, bool enable /*= true*/)
{
	if (enable)
		SingletonContainer::instance().get<Timer>()->registerUpdateObserver(scene);
	else
		SingletonContainer::instance().get<Timer>()->removeUpdateObserver(scene);
}

void SceneStack::impl::pushSceneToDirector(GameObject *scene)
{
	auto director = cocos2d::Director::getInstance();
	if (director->getRunningScene())
		director->pushScene(scene->getComponent<DisplayNode>()->getAs<DisplayNode::Scene>());
	else
		director->runWithScene(scene->getComponent<DisplayNode>()->getAs<DisplayNode::Scene>());
}

GameObject * SceneStack::impl::topScene()
{
	if (m_scenes.empty())
		throw ("Getting topScene while there are no scenes.");

	return m_scenes.back().get();
}


SceneStack::SceneStack() :Object("SceneStack"), pimpl(new impl)
{

}

SceneStack::~SceneStack()
{

}

GameObject* SceneStack::pushAndRun(std::unique_ptr<GameObject> &&scene)
{
	if (!scene || !scene->getComponent<DisplayNode>() || !scene->getComponent<DisplayNode>()->getAs<DisplayNode::Scene>())
		throw("pushScene with a non-scene");
	if (scene->getParent())
		throw("pushScene with a game object that has a parent");

	//Disable update for old top scene, then push the param scene into stack and enable it.
	if (!pimpl->m_scenes.empty())
		pimpl->switchUpdate(pimpl->topScene(), false);
	pimpl->m_scenes.emplace_back(std::move(scene));
	pimpl->switchUpdate(pimpl->topScene());
	pimpl->pushSceneToDirector(pimpl->topScene());

	return pimpl->topScene();
}

std::unique_ptr<GameObject> SceneStack::pop()
{
	if (pimpl->m_scenes.size() <= 1)
		throw("popScene when the size of scenes <= 1");

	//Disable update for the old top scene, then pop it and enable the new top scene.
	pimpl->switchUpdate(pimpl->topScene(), false);
	auto ownership = std::unique_ptr<GameObject>(std::move(pimpl->m_scenes.back()));
	pimpl->m_scenes.pop_back();
	pimpl->switchUpdate(pimpl->topScene());
	cocos2d::Director::getInstance()->popScene();

	return ownership;
}

GameObject* SceneStack::getCurrentScene()
{
	return pimpl->topScene();
}
