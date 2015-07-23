#ifndef __DISPLAY_NODE__
#define __DISPLAY_NODE__

#include <memory>
#include <functional>

#include "ActorComponent.h"

class DisplayNode final : public ActorComponent
{
	friend class Actor;

public:
	DisplayNode();
	~DisplayNode();

	//Create an instance.
	static std::unique_ptr<DisplayNode> create();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;

	DisplayNode *getParent() const;

	//////////////////////////////////////////////////////////////////////////
	//Initializer and getter for underlying object
	//////////////////////////////////////////////////////////////////////////

	//T must be subclass of cocos2d::Node.
	//In most cases, just leave the creater_func blank:
	//initAs<cocos2d::Scene>();
	//You can supply creater_func with a lambda, which will be used for creating the underlying object:
	//initAs<cocos2d::Sprite>([](){return cocos2d::Sprite::create("some_file_name");});
	template<typename T>
	T* initAs(std::function<T*()> &&creater_func = nullptr)
	{
		return static_cast<T*>(initAsHelper(creater_func ? std::move(creater_func) : [](){return T::create(); }));
	}

	template<typename T>
	T* getAs() const
	{
		return static_cast<T*>(getNode());
	}

	//Disable copy/move constructor and operator=.
	DisplayNode(const DisplayNode&) = delete;
	DisplayNode(DisplayNode&&) = delete;
	DisplayNode& operator=(const DisplayNode&) = delete;
	DisplayNode& operator=(DisplayNode&&) = delete;

private:
	DisplayNode(Actor *game_object);

	void addChild(DisplayNode *child);
	void removeFromParent();

	//Get the internal cocos2d::Node. 
	void * getNode() const;

	//Helper function for initAs().
	void * initAsHelper(std::function<void*()> && creatorFunction);

	//Override functions.
	virtual const std::string & getType() const override;
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;

	//Implementation stuff.
	struct DisplayNodeImpl;
	std::unique_ptr<DisplayNodeImpl> pimpl;
};

#endif // !__DISPLAY_NODE__
