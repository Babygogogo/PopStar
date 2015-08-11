#ifndef __GENERAL_RENDER_COMPONENT__
#define __GENERAL_RENDER_COMPONENT__

#include <memory>
#include <functional>

#include "BaseRenderComponent.h"

class GeneralRenderComponent final : public BaseRenderComponent
{
	friend class Actor;

public:
	GeneralRenderComponent();
	GeneralRenderComponent(Actor *game_object);
	~GeneralRenderComponent();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;

	GeneralRenderComponent *getParent() const;

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
		return static_cast<T*>(getSceneNode());
	}

	//Disable copy/move constructor and operator=.
	GeneralRenderComponent(const GeneralRenderComponent&) = delete;
	GeneralRenderComponent(GeneralRenderComponent&&) = delete;
	GeneralRenderComponent& operator=(const GeneralRenderComponent&) = delete;
	GeneralRenderComponent& operator=(GeneralRenderComponent&&) = delete;

private:
	void addChild(GeneralRenderComponent *child);
	void removeFromParent();

	//Helper function for initAs().
	cocos2d::Node * initAsHelper(std::function<void*()> && creatorFunction);

	//Override functions.
	virtual const std::string & getType() const override;
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;

	//Implementation stuff.
	struct DisplayNodeImpl;
	std::unique_ptr<DisplayNodeImpl> pimpl;
};

#endif // !__GENERAL_RENDER_COMPONENT__
