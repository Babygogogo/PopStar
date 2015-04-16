#ifndef __BW_DISPLAY_NODE__
#define __BW_DISPLAY_NODE__

#include "cocos2d.h"
#include <memory>
#include "Component.h"

class DisplayNode final : public Component
{
	friend class GameObject;

public:
	using Node = cocos2d::Node;
	using Sprite = cocos2d::Sprite;
	using Layer = cocos2d::Layer;
	using Scene = cocos2d::Scene;

	virtual ~DisplayNode();

	DisplayNode *getParent() const;

	template<typename T>
	T* initAs()
	{
		if (auto existing_node = getAs<T>())
			return existing_node;
		if (m_node)
			throw("Initializing a initialized DisplayNode");

		this->m_node = T::create();
		if (auto parent = getParent()){
			if (parent->m_node)
				parent->m_node->addChild(this->m_node);
			else
				parent->initAs<DefaultNode>()->addChild(this->m_node);
		}

		this->m_node->retain();
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
	using DefaultNode = cocos2d::Sprite;

	DisplayNode(GameObject *game_object);

	void addChild(DisplayNode *child);
	void removeFromParent();

	Node* m_node{ nullptr };

	class impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__BW_DISPLAY_NODE__
