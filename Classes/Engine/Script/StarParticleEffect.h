#ifndef __STAR_PARTICLE_EFFECT__
#define __STAR_PARTICLE_EFFECT__

#include "../Actor/BaseScriptComponent.h"

#include <memory>

class Star;

class StarParticleEffect final :public BaseScriptComponent
{
	friend class Actor;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StarParticleEffect(const StarParticleEffect&) = delete;
	StarParticleEffect(StarParticleEffect&&) = delete;
	StarParticleEffect& operator=(const StarParticleEffect&) = delete;
	StarParticleEffect& operator=(StarParticleEffect&&) = delete;

public:
	StarParticleEffect(Actor *game_object);
	~StarParticleEffect();

	static const std::string Type;

	void reset(Star *star);

	virtual const std::string & getType() const override;

private:

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR_PARTICLE_EFFECT__
