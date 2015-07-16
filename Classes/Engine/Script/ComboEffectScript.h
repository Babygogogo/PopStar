#ifndef __COMBO_EFFECT__
#define __COMBO_EFFECT__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class ComboEffectScript final : public BaseScriptComponent
{
	friend class Actor;
	
public:
	~ComboEffectScript();

	//Create an instance.
	static std::unique_ptr<ComboEffectScript> create();

	//The type name of this component. Must be the same as the class name.
	static const std::string Type;

	//Disable copy/move constructor and operator=.
	ComboEffectScript(const ComboEffectScript&) = delete;
	ComboEffectScript(ComboEffectScript&&) = delete;
	ComboEffectScript& operator=(const ComboEffectScript&) = delete;
	ComboEffectScript& operator=(ComboEffectScript&&) = delete;

private:
	ComboEffectScript();
	ComboEffectScript(Actor *game_object);

	virtual const std::string & getType() const override;

	//Implementation stuff.
	struct ComboEffectImpl;
	std::unique_ptr<ComboEffectImpl> pimpl;
};

#endif // !__COMBO_EFFECT__
