#ifndef __COMBO_EFFECT__
#define __COMBO_EFFECT__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class ComboEffect final : public BaseScriptComponent
{
	friend class Actor;
	
public:
	~ComboEffect();

	//Create an instance.
	static std::unique_ptr<ComboEffect> create();

	static const std::string Type;

	//Disable copy/move constructor and operator=.
	ComboEffect(const ComboEffect&) = delete;
	ComboEffect(ComboEffect&&) = delete;
	ComboEffect& operator=(const ComboEffect&) = delete;
	ComboEffect& operator=(ComboEffect&&) = delete;

private:
	ComboEffect();
	ComboEffect(Actor *game_object);

	virtual const std::string & getType() const override;

	struct ComboEffectImpl;
	std::unique_ptr<ComboEffectImpl> pimpl;
};

#endif // !__COMBO_EFFECT__
