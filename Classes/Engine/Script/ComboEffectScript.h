#ifndef __COMBO_EFFECT_SCRIPT__
#define __COMBO_EFFECT_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

/*!
 * \class ComboEffectScript
 *
 * \brief Manages the effect that is shown to praise the player when he makes a great move.
 *
 * \details
 *	It's hard to make a good name for this class...
 *
 * \author Babygogogo
 * \date 2015.7
 */
class ComboEffectScript final : public BaseScriptComponent
{
public:
	ComboEffectScript();
	~ComboEffectScript();

	//The type name of this component. Same as the class name.
	static const std::string Type;

	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	ComboEffectScript(const ComboEffectScript&) = delete;
	ComboEffectScript(ComboEffectScript&&) = delete;
	ComboEffectScript& operator=(const ComboEffectScript&) = delete;
	ComboEffectScript& operator=(ComboEffectScript&&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;
	virtual void vPostInit() override;

	//Implementation stuff.
	struct ComboEffectScriptImpl;
	std::shared_ptr<ComboEffectScriptImpl> pimpl;
};

#endif // !__COMBO_EFFECT_SCRIPT__
