#include "Obstacle.h"
#include "ObstacleData.h"
#include "globals.h"
#include "lists.h"
#include "Alegrinho.h"
#include "GameManager.h"

Obstacle::Obstacle(): WorldObject()
{
	
}


Obstacle::Obstacle(int x, int y, uint8_t type, uint8_t variant): WorldObject(x, y, type, variant)
{
	init();
}


/*
Obstacle::Obstacle(float x, float y, char type): WorldObject(x, y, type)
{
	init();
}*/




/*
int Obstacle::get_total_animation_frames() const
{
	return (myData ? std::max( (int)myData->animationFrames, 1) : 1);
}


int Obstacle::get_animation_delay() const
{
	return (myData ? (int)myData->animationDelay : 0);
}


int Obstacle::get_variants_count() const
{
	return (myData ? std::max( (int)myData->variants, 1) : 1);
}


float Obstacle::get_width() const
{
	//return (float)get_tile_width() * (float)TILE_SIZE;
	return (myData ? myData->radius * 2.0f : (float)TILE_SIZE * 3.0f);
}


int Obstacle::get_tile_width() const
{
	return (int)(get_width() / (float)TILE_SIZE);
}



bool Obstacle::tile_collision(int x, int y) const
{
    
    const int width = get_tile_width();
    IntVector2 Obstacle_tile = get_tile_position();
    return point_to_rectangle_collision(x, y, Obstacle_tile.x, Obstacle_tile.y, width, width);
    
}
*/

/*
void Obstacle::update()
{
	
	int total_animation_frames = get_total_animation_frames();
	if (total_animation_frames > 1)
	{
		
		if (animationDelay > 0)
			animationDelay -= 1;
		
		else //if (animationDelay <= 0)
		{
			
			animationDelay = get_animation_delay();
			animationFrame++;
			if (animationFrame >= total_animation_frames)
				animationFrame = 0;
			
		}
		
	}
	
}*/


void Obstacle::draw() const
{
	
	Bitmap * sprite = myData ? &myData->sprite : &missingSprite; //&donutSprite;
	if (sprite)
	{
		
		const int total_variants = get_variants_count();
		const int total_animation_frames = get_total_animation_frames();
		
		const float base_src_width = sprite->get_width();
		const float base_src_height = sprite->get_height();
		
		const float variant_src_width = base_src_width / (float)total_variants;
		
		const float src_width = variant_src_width;
		const float src_height = base_src_height / (float)total_animation_frames;
		
		const float src_x = (float)get_variant_index() * variant_src_width;
		const float src_y = (float)animationFrame * src_height;
		
		const float draw_width = src_width;
		const float draw_height = src_height;
		
		//const float draw_x = position.x - draw_width * 0.5f; //(float)this->x * TILE_SIZE + TILE_SIZE * 0.5f;
		//const float draw_y = position.y - draw_height * 0.5f; //(float)this->y * TILE_SIZE + TILE_SIZE * 0.5f;
		
		/*
		const int8_t pivot = myData ? myData->pivot : 0;
		float draw_x, draw_y;
		if (is_bit_flag_set(pivot, PIVOT_LEFT) )
			draw_x = position.x;
			
		else if (is_bit_flag_set(pivot, PIVOT_RIGHT) )
			draw_x = position.x - draw_width;
			
		else // center
			draw_x = position.x - draw_width * 0.5f;
		
		
		if (is_bit_flag_set(pivot, PIVOT_TOP) )
			draw_y = position.y;
			
		else if (is_bit_flag_set(pivot, PIVOT_BOTTOM) )
			draw_y = position.y - draw_height;
			
		else // center
			draw_y = position.y - draw_height * 0.5f;
		*/
		
		Vector2 draw_position = get_draw_position(draw_width, draw_height);
		
		spriteCache.draw_sprite_region(*sprite, src_x, src_y, src_width, src_height, draw_position.x, draw_position.y, draw_width, draw_height);
	
	}
	
}




// private
/*
void Obstacle::init()
{
	
	if (link_data() )
	{
		
		const int variants_count = get_variants_count();
		variantIndex = variants_count > 1 ? iRand(0, variants_count - 1) : 0;
		
	}
	
}*/



bool Obstacle::link_data()
{
	
	for (ObstacleData & current : gameManager->obstacleData)
	{
		if (current.id == this->type)
		{
			myData = &current;
			return true;
		}
	}
	
	return false;
	
}


/*
void Obstacle::debug_printf()
{
    
    GameObject::debug_printf();
    printf("get_width(): %.2f, get_tile_width(): %d\n", get_width(), get_tile_width() );
    
}*/


