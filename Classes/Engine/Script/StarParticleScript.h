#ifndef __STAR_PARTICLE_SCRIPT__
#define __STAR_PARTICLE_SCRIPT__

#include <memory>

#include "../Actor/BaseScriptComponent.h"

class StarScript;

class StarParticleScript final :public BaseScriptComponent
{
public:
	StarParticleScript();
	~StarParticleScript();

	void reset(StarScript *star);

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
	virtual void vPostInit() override;

	//Implementation stuff.
	struct StarParticleScriptImpl;
	std::unique_ptr<StarParticleScriptImpl> pimpl;
};

#endif // !__STAR_PARTICLE_SCRIPT__
