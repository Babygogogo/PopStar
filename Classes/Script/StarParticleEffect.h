#ifndef __STAR_PARTICLE_EFFECT__
#define __STAR_PARTICLE_EFFECT__

#include "Script.h"

#include <memory>

class Star;

class StarParticleEffect final :public Script
{
	friend class GameObject;

	//////////////////////////////////////////////////////////////////////////
	//Disable copy/move constructor and operator=.
	//////////////////////////////////////////////////////////////////////////
	StarParticleEffect(const StarParticleEffect&) = delete;
	StarParticleEffect(StarParticleEffect&&) = delete;
	StarParticleEffect& operator=(const StarParticleEffect&) = delete;
	StarParticleEffect& operator=(StarParticleEffect&&) = delete;
	
public:
	~StarParticleEffect();

	void reset(Star *star);

private:
	StarParticleEffect(GameObject *game_object);

	struct impl;
	std::unique_ptr<impl> pimpl;
};

#endif // !__STAR_PARTICLE_EFFECT__
