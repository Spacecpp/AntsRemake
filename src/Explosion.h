#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"

#define EXPLOSION_TOTAL_ANIMATION_FRAMES 5
#define EXPLOSION_ANIMATION_DELAY 5


class Explosion: public GameObject
{
	
	public:
		int8_t animationFrame;
		int8_t animationDelay;
		
		//Explosion();
		//Explosion(float x, float y);
		Explosion(int x, int y);
		
		void update();
		void draw() const;
	
};

#endif
