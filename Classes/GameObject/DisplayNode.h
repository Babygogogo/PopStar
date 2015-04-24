#ifndef __BW_DISPLAY_NODE__
#define __BW_DISPLAY_NODE__

#include <memory>
#include <functional>
#include "cocos2d.h"
#include "Component.h"

class DisplayNode final : public Component
{
	friend class GameObject;

public:
	virtual ~DisplayNode();

	DisplayNode *getParent() const;

	//////////////////////////////////////////////////////////////////////////
	//Initializer and getter for underlying object
	//////////////////////////////////////////////////////////////////////////

	//*	T must be subclass of cocos2d::Node.
	//*	In most cases, just leave the creater_func blank:
	//initAs<cocos2d::Scene>();
	//*	You can supply creater_func with a lambda, which will be used for creating the underlying object:
	//initAs<cocos2d::Sprite>([](){return cocos2d::Sprite::create("some_file_name");});
	template<typename T>
	T* initAs(std::function<T*()> &&creater_func = nullptr)
	{
		if (m_node)
			throw("Initializing a initialized DisplayNode");

		this->m_node = creater_func ? creater_func() : T::create();
		this->m_node->retain();
		attachToParent();

		return getAs<T>();
	}

	template<typename T>
	T* getAs() const
	{
		return dynamic_cast<T*>(m_node);
	}

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	DisplayNode(const DisplayNode&) = delete;
	DisplayNode(DisplayNode&&) = delete;
	DisplayNode& operator=(const DisplayNode&) = delete;
	DisplayNode& operator=(DisplayNode&&) = delete;

private:
	DisplayNode(GameObject *game_object);

	void addChild(DisplayNode *child);
	void attachToParent();
	void removeFromParent();

	cocos2d::Node* m_node{ nullptr };

	class impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__BW_DISPLAY_NODE__
