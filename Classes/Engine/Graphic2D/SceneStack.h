#ifndef __SCENE_STACK__
#define __SCENE_STACK__

#include <memory>

class Actor;

/*!
 * \class	SceneStack
 *
 * \brief	A singleton class that manages the roots of the GameObjects (e.g. the scenes).
 * \details
 *	The top scene is also the running one. There's no way to run a scene that is not at the top of the stack.
 *	The top scene (and only) will be update once a frame.
 *	If the last scene is popped, an exception will be thrown and the application will be terminated.
 *	To save memory, don't make the length of the stack too long.
 *
 * \author	Babygogogo
 * \date	2015.3
 */
class SceneStack final
{
public:
	//You can't create two or more SceneStacks, otherwise an assertion will be triggered.
	SceneStack();
	~SceneStack();

	//////////////////////////////////////////////////////////////////////////
	//Stuff to control the stack.
	//////////////////////////////////////////////////////////////////////////
	//The param scene must be a scene and has no parent, or an exception will be thrown.
	Actor* pushAndRun(std::shared_ptr<Actor> &&scene);

	//The ownership of the pre-top scene is returned. You can do anything with it, or just leave it alone.
	std::shared_ptr<Actor> pop();

	//Replace the current scene with a new one. The ownership of the pre-current scene will be returned.
	std::shared_ptr<Actor> replaceAndRun(std::shared_ptr<Actor> &&scene);

	//The current scene is also the top scene in the stack.
	Actor* getCurrentScene();

	//Disable copy/move constructor and operator=.
	SceneStack(const SceneStack&) = delete;
	SceneStack(SceneStack&&) = delete;
	SceneStack& operator=(const SceneStack&) = delete;
	SceneStack& operator=(SceneStack&&) = delete;

private:
	struct SceneStackImpl;
	std::unique_ptr<SceneStackImpl> pimpl;
};

#endif // !__SCENE_STACK__
