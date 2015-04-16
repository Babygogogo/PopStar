#include <vector>
#include "SceneStack.h"
#include "SingletonContainer.h"
#include "Timer.h"
#include "../GameObject/DisplayNode.h"

struct SceneStack::impl
{
	impl();
	~impl();

	void validate(GameObject* scene);
	void switchUpdate(GameObject *scene, bool enable = true);

	void push(std::unique_ptr<GameObject> &&scene);
	std::unique_ptr<GameObject> pop();

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

void SceneStack::impl::validate(GameObject* scene)
{
	if (!scene || !scene->getComponent<DisplayNode>() || !scene->getComponent<DisplayNode>()->getAs<DisplayNode::Scene>())
		throw("pushScene with a non-scene");
	if (scene->getParent())
		throw("pushScene with a game object that has a parent");
}

void SceneStack::impl::push(std::unique_ptr<GameObject> &&scene)
{
	m_scenes.emplace_back(std::move(scene));
	switchUpdate(topScene());
}

std::unique_ptr<GameObject> SceneStack::impl::pop()
{
	switchUpdate(topScene(), false);
	auto ownership = std::unique_ptr<GameObject>(std::move(m_scenes.back()));
	m_scenes.pop_back();

	return ownership;
}


SceneStack::SceneStack() :Object("SceneStack"), pimpl(new impl)
{

}

SceneStack::~SceneStack()
{

}

GameObject* SceneStack::pushAndRun(std::unique_ptr<GameObject> &&scene)
{
	pimpl->validate(scene.get());

	//Disable update for old top scene, then push the param scene into stack and enable it.
	if (!pimpl->m_scenes.empty())
		pimpl->switchUpdate(pimpl->topScene(), false);
	pimpl->push(std::move(scene));

	pimpl->pushSceneToDirector(pimpl->topScene());

	return pimpl->topScene();
}

std::unique_ptr<GameObject> SceneStack::pop()
{
	if (pimpl->m_scenes.size() <= 1)
		throw("popScene when the size of scenes <= 1");

	//Pop the current scene and enable the new top scene.
	auto ownership = pimpl->pop();
	pimpl->switchUpdate(pimpl->topScene());

	cocos2d::Director::getInstance()->popScene();

	return ownership;
}

GameObject* SceneStack::getCurrentScene()
{
	return pimpl->topScene();
}

std::unique_ptr<GameObject> SceneStack::replaceAndRun(std::unique_ptr<GameObject> &&scene)
{
	pimpl->validate(scene.get());
	if (pimpl->m_scenes.empty())
		throw("replaceAndRun scene when there are no scenes at all");

	auto ownership = pimpl->pop();
	pimpl->push(std::move(scene));

	cocos2d::Director::getInstance()->replaceScene(pimpl->topScene()->getComponent<DisplayNode>()->getAs<DisplayNode::Scene>());

	return ownership;
}
