#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include "GameObject.h"
class Bitmap;


namespace ANIMATED_SPRITE { enum { EXPLOSION, DEATH_SMOKE }; }

class AnimatedSprite: public GameObject
{
	
	public:
		//int8_t totalFrames;
		int8_t type;
		int8_t animationFrame;
		int8_t animationDelay;
		
		AnimatedSprite(int x, int y, int type);
		AnimatedSprite(float x, float y, int type);
		
		void update();
		void draw() const;
		
		
		int8_t get_total_animation_frames() const;
		int8_t get_delay_per_frame() const;
		Bitmap * get_sprite() const;
	
};

#endif
