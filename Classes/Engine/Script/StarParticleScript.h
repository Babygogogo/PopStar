#ifndef __STAR_PARTICLE_SCRIPT__
#define __STAR_PARTICLE_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

//Forward declaration.
namespace cocos2d{
	struct Color4F;
}

class StarParticleScript final :public BaseScriptComponent
{
public:
	StarParticleScript();
	~StarParticleScript();

	void show(float posX, float posY, const cocos2d::Color4F & color);

	static const std::string Type;
	virtual const std::string & getType() const override;

	//Disable copy/move constructor and operator=.
	StarParticleScript(const StarParticleScript&) = delete;
	StarParticleScript(StarParticleScript&&) = delete;
	StarParticleScript& operator=(const StarParticleScript&) = delete;
	StarParticleScript& operator=(StarParticleScript&&) = delete;

private:
	//Override functions.
	virtual bool vInit(tinyxml2::XMLElement *xmlElement) override;

	//Implementation stuff.
	struct StarParticleScriptImpl;
	std::unique_ptr<StarParticleScriptImpl> pimpl;
};

#endif // !__STAR_PARTICLE_SCRIPT__
