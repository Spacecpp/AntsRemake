#include "AnimatedSprite.h"
#include "globals.h"


//#define EXPLOSION_TOTAL_ANIMATION_FRAMES 5
//#define EXPLOSION_ANIMATION_DELAY 5

#define DEATH_SMOKE_TOTAL_ANIMATION_FRAMES 7
#define DEATH_SMOKE_ANIMATION_DELAY 5



AnimatedSprite::AnimatedSprite(int x, int y, int type): GameObject(x, y), type(type), animationFrame(0)
{
	animationDelay = get_delay_per_frame();
}


AnimatedSprite::AnimatedSprite(float x, float y, int type): GameObject(x, y), type(type), animationFrame(0)
{
	animationDelay = get_delay_per_frame();
}



void AnimatedSprite::update()
{
	
	animationDelay--;
	if (animationDelay <= 0)
	{
		
		animationDelay = get_delay_per_frame();
		
		animationFrame++;
		if (animationFrame >= get_total_animation_frames() )
			destroy();
		
	}
	
}



void AnimatedSprite::draw() const
{
	
	Bitmap * sprite = get_sprite();
	if (!sprite) return;
	
	const float src_width = (float)sprite->get_width() / (float)get_total_animation_frames();
	const float src_height = (float)sprite->get_height();
	const float draw_width = src_width;
    const float draw_height = src_height;
	//const float half_width = src_width * 0.5f;
	//const float half_height = src_height * 0.5f;
	
	const float draw_x = position.x - draw_width * 0.5f; //(float)this->x * TILE_SIZE + TILE_SIZE * 0.5f;
	const float draw_y = position.y - draw_height * 0.5f; //(float)this->y * TILE_SIZE + TILE_SIZE * 0.5f;
	
	const float src_x = (float)animationFrame * src_width;
	
	spriteCache.draw_sprite_region(*sprite, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
	
}



int8_t AnimatedSprite::get_total_animation_frames() const
{
	
	if (type == 0) return global_explosionAnimationFrames; //EXPLOSION_TOTAL_ANIMATION_FRAMES;
	if (type == 1) return DEATH_SMOKE_TOTAL_ANIMATION_FRAMES;
	return 1;
	
}



int8_t AnimatedSprite::get_delay_per_frame() const
{
	
	if (type == 0) return global_explosionAnimationDelay; //EXPLOSION_ANIMATION_DELAY;
	if (type == 1) return DEATH_SMOKE_ANIMATION_DELAY;
	return 1;
	
}


Bitmap * AnimatedSprite::get_sprite() const
{
	
	if (type == 0) return &explosionSprite;
	if (type == 1) return &deathSmokeSprite;
	return NULL;
	
}

