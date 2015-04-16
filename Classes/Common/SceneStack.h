#ifndef __BW_SCENE_STACK__
#define __BW_SCENE_STACK__

#include <memory>
#include "Object.h"
#include "../GameObject/GameObject.h"

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
class SceneStack final : public Object
{
	friend class SingletonContainer;

public:
	virtual ~SceneStack();

	//////////////////////////////////////////////////////////////////////////
	//Stuff to control the stack.
	//////////////////////////////////////////////////////////////////////////
	//The param scene must be a scene and has no parent, or an exception will be thrown.
	GameObject* pushAndRun(std::unique_ptr<GameObject> &&scene);
	//The ownership of the pre-top scene is returned. You can do anything with it, or just leave it alone.
	std::unique_ptr<GameObject> pop();
	//Replace the current scene with a new one. The ownership of the pre-current scene will be returned.
	std::unique_ptr<GameObject> replaceAndRun(std::unique_ptr<GameObject> &&scene);
	//The current scene is also the top scene in the stack.
	GameObject* getCurrentScene();

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	SceneStack(const SceneStack&) = delete;
	SceneStack(SceneStack&&) = delete;
	SceneStack& operator=(const SceneStack&) = delete;
	SceneStack& operator=(SceneStack&&) = delete;

private:
	//SceneStack is singleton. To get its instance, use SingletonContainer.
	SceneStack();

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif
