#include "Explosion.h"
#include "globals.h"

Explosion::Explosion(int x, int y): GameObject(x, y), animationFrame(0), animationDelay(EXPLOSION_ANIMATION_DELAY)
{
	
}



void Explosion::update()
{
	
	animationDelay--;
	if (animationDelay <= 0)
	{
		
		animationDelay = EXPLOSION_ANIMATION_DELAY;
		
		animationFrame++;
		if (animationFrame >= EXPLOSION_TOTAL_ANIMATION_FRAMES)
			destroy();
		
	}
	
}



void Explosion::draw() const
{
	
	Bitmap * sprite = &explosionSprite;
	
	const float src_width = sprite->get_width() / EXPLOSION_TOTAL_ANIMATION_FRAMES;
	const float src_height = sprite->get_height();
	const float draw_width = src_width;
    const float draw_height = src_height;
	//const float half_width = src_width * 0.5f;
	//const float half_height = src_height * 0.5f;
	
	const float draw_x = position.x - draw_width * 0.5f; //(float)this->x * TILE_SIZE + TILE_SIZE * 0.5f;
	const float draw_y = position.y - draw_height * 0.5f; //(float)this->y * TILE_SIZE + TILE_SIZE * 0.5f;
	
	const float src_x = (float)animationFrame * src_width;
	
	//DrawTexturePro(*sprite, Rectangle { src_x, 0.0f, src_width, src_height }, Rectangle { draw_x, draw_y, src_width, src_height }, Vector2 { half_width, half_height }, 0.0f, WHITE);
	
	spriteCache.draw_sprite_region(*sprite, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
	
}

