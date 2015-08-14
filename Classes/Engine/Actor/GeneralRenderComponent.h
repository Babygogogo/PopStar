#ifndef __GENERAL_RENDER_COMPONENT__
#define __GENERAL_RENDER_COMPONENT__

#include <memory>

#include "BaseRenderComponent.h"

class GeneralRenderComponent final : public BaseRenderComponent
{
	friend class Actor;

public:
	GeneralRenderComponent();
	~GeneralRenderComponent();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;

	virtual const std::string & getType() const override;

	GeneralRenderComponent *getParent() const;

	//Getter for underlying object which automatically downcasts the pointer to the type you specified.
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
	//#TODO: These functions should be in BaseRenderComponent or be removed.
	void addChild(GeneralRenderComponent *child);
	void removeFromParent();

	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;

	//Implementation stuff.
	struct GeneralRenderComponentImpl;
	std::unique_ptr<GeneralRenderComponentImpl> pimpl;
};

#endif // !__GENERAL_RENDER_COMPONENT__
