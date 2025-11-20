#ifndef HEAL_PARTICLE_H
#define HEAL_PARTICLE_H

#include "GameObject.h"

#define HEAL_PARTICLE_MAX_LIFETIME 120


class HealParticle: public GameObject
{
	
	protected:
		
	private:
		int16_t lifetime; 
		
	public:
		HealParticle(float x, float y);
		
		void update();
		void draw() const;
	
};
#endif 
