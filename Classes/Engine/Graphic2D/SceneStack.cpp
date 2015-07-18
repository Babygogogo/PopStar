#include <vector>
#include "SceneStack.h"
#include "../Utilities/SingletonContainer.h"
#include "../MainLoop/Timer.h"
#include "../Actor/DisplayNode.h"
#include "cocos2d.h"

struct SceneStack::impl
{
	impl();
	~impl();

	void validate(Actor* scene);
	void switchUpdate(Actor *scene, bool enable = true);

	void push(std::unique_ptr<Actor> &&scene);
	std::unique_ptr<Actor> pop();

	void pushSceneToDirector(Actor *scene);
	Actor *topScene();

	std::vector<std::unique_ptr<Actor>> m_scenes;
};

SceneStack::impl::impl()
{

}

SceneStack::impl::~impl()
{

}

void SceneStack::impl::switchUpdate(Actor *scene, bool enable /*= true*/)
{
	if (enable)
		SingletonContainer::getInstance()->get<Timer>()->registerUpdateObserver(scene);
	else
		SingletonContainer::getInstance()->get<Timer>()->removeUpdateObserver(scene);
}

void SceneStack::impl::pushSceneToDirector(Actor *scene)
{
	auto director = cocos2d::Director::getInstance();
	if (director->getRunningScene())
		director->pushScene(scene->getComponent<DisplayNode>()->getAs<cocos2d::Scene>());
	else
		director->runWithScene(scene->getComponent<DisplayNode>()->getAs<cocos2d::Scene>());
}

Actor * SceneStack::impl::topScene()
{
	if (m_scenes.empty())
		throw ("Getting topScene while there are no scenes.");

	return m_scenes.back().get();
}

void SceneStack::impl::validate(Actor* scene)
{
	if (!scene || !scene->getComponent<DisplayNode>() || !scene->getComponent<DisplayNode>()->getAs<cocos2d::Scene>())
		throw("pushScene with a non-scene");
	if (scene->getParent())
		throw("pushScene with a game object that has a parent");
}

void SceneStack::impl::push(std::unique_ptr<Actor> &&scene)
{
	m_scenes.emplace_back(std::move(scene));
	switchUpdate(topScene());
}

std::unique_ptr<Actor> SceneStack::impl::pop()
{
	switchUpdate(topScene(), false);
	auto ownership = std::unique_ptr<Actor>(std::move(m_scenes.back()));
	m_scenes.pop_back();

	return ownership;
}


SceneStack::SceneStack() :Object("SceneStack"), pimpl(new impl)
{

}

SceneStack::~SceneStack()
{

}

std::unique_ptr<SceneStack> SceneStack::create()
{
	return std::unique_ptr<SceneStack>(new SceneStack());
}

Actor* SceneStack::pushAndRun(std::unique_ptr<Actor> &&scene)
{
	pimpl->validate(scene.get());

	//Disable update for old top scene, then push the param scene into stack and enable it.
	if (!pimpl->m_scenes.empty())
		pimpl->switchUpdate(pimpl->topScene(), false);
	pimpl->push(std::move(scene));

	pimpl->pushSceneToDirector(pimpl->topScene());

	return pimpl->topScene();
}

std::unique_ptr<Actor> SceneStack::pop()
{
	if (pimpl->m_scenes.size() <= 1)
		throw("popScene when the size of scenes <= 1");

	//Pop the current scene and enable the new top scene.
	auto ownership = pimpl->pop();
	pimpl->switchUpdate(pimpl->topScene());

	cocos2d::Director::getInstance()->popScene();

	return ownership;
}

Actor* SceneStack::getCurrentScene()
{
	return pimpl->topScene();
}

std::unique_ptr<Actor> SceneStack::replaceAndRun(std::unique_ptr<Actor> &&scene)
{
	pimpl->validate(scene.get());
	if (pimpl->m_scenes.empty())
		throw("replaceAndRun scene when there are no scenes at all");

	auto ownership = pimpl->pop();
	pimpl->push(std::move(scene));

	cocos2d::Director::getInstance()->replaceScene(pimpl->topScene()->getComponent<DisplayNode>()->getAs<cocos2d::Scene>());

	return ownership;
}
