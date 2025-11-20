#include "Food.h"
#include "FoodData.h"
#include "globals.h"
#include "lists.h"
#include "Alegrinho.h"
#include "GameManager.h"

Food::Food(): WorldObject(), points(0)
{
	
}


Food::Food(int x, int y, uint8_t type, uint8_t variant): WorldObject(x, y, type, variant), points(0)
{
	init();
}


/*
Food::Food(float x, float y, char type): WorldObject(x, y, type), points(0)
{
	init();
}*/




int Food::get_points_per_piece() const
{
	return (myData ? (static_cast<FoodData*>(myData) )->points : 10); 
}


int Food::get_total_pieces() const
{
	return (myData ? (static_cast<FoodData*>(myData) )->parts : 10);
}



ColorRGBA Food::get_piece_color() const
{
	return (myData ? (static_cast<FoodData*>(myData) )->partColor : WHITE);
}


int Food::get_total_eat_frames() const
{
	return (myData ? std::max( (int)(static_cast<FoodData*>(myData) )->eatFrames, 1) : 1);
}


/*
int Food::get_total_animation_frames() const
{
	return (myData ? std::max( (int)myData->animationFrames, 1) : 1);
}


int Food::get_animation_delay() const
{
	return (myData ? (int)myData->animationDelay : 0);
}


int Food::get_variants_count() const
{
	return (myData ? std::max( (int)myData->variants, 1) : 1);
}


float Food::get_width() const
{
	//return (float)get_tile_width() * (float)TILE_SIZE;
	return (myData ? myData->radius * 2.0f : (float)TILE_SIZE * 3.0f);
}


int Food::get_tile_width() const
{
	//return 3;
	return (int)(get_width() / (float)TILE_SIZE);
}



bool Food::tile_collision(int x, int y) const
{
    
    const int width = get_tile_width();
    IntVector2 food_tile = get_tile_position();
    return point_to_rectangle_collision(x, y, food_tile.x, food_tile.y, width, width);
    
}
*/


int Food::get_current_eat_frame() const
{
	
	const int total_pieces = get_total_pieces();
	if (points >= total_pieces) return 0; // optimization
	
	const int total_eat_frames = get_total_eat_frames();
	if (total_eat_frames <= 1) return 0; // optimization
	
	/*
	const float factor = (float)points / (float)total_pieces; // how much % of the food remains
	const float slice = 1.0f / (float)total_eat_frames;
	float value = 1.0f;
	
	for (int i = 0; i < total_eat_frames; i++)
	{
		if (value <= factor)
			return i;
		value -= slice;
	}
	*/
	
	int value = total_pieces;
	int slice = total_pieces / total_eat_frames;
	
	for (int i = 0; i < total_eat_frames; i++)
	{
		if (value <= points || points > value - slice)
			return i;
		value -= slice;
	}
	
	return total_eat_frames - 1; // safety
	
}




void Food::draw() const
{
	
	Bitmap * sprite = myData ? &myData->sprite : &missingSprite; //&donutSprite;
	if (sprite)
	{
		
		const int total_eat_frames = get_total_eat_frames();
		const int total_variants = get_variants_count();
		const int total_animation_frames = get_total_animation_frames();
		
		const float base_src_width = sprite->get_width();
		const float base_src_height= sprite->get_height();
		
		const float variant_src_width = base_src_width / (float)total_variants;
		
		const float src_width = variant_src_width / (float)total_eat_frames;
		const float src_height = base_src_height / (float)total_animation_frames;
		
		const float src_x = (float)get_current_eat_frame() * src_width + (float)get_variant_index() * variant_src_width;
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


// net
void Food::pack_resync_data(BitStream & data)
{
    data.addInt(points, 2);
}



void Food::unpack_resync_data(BitStream & _data)
{
    
    points = _data.getInt(2);
    active = points > 0;
    
}


// private
void Food::init()
{
	
	if (link_data() )
	{
		
		this->points = (static_cast<FoodData*>(myData) )->parts;
		
		if (variant == 0)
		{
			const int variants_count = get_variants_count();
			variant = iRand(0, variants_count - 1) + 1;
		}
		
	}
	
}


bool Food::link_data()
{
	
	for (FoodData & current : gameManager->foodData)
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
void Food::debug_printf()
{
    
    GameObject::debug_printf();
    printf("get_width(): %.2f, get_tile_width(): %d\n", get_width(), get_tile_width() );
    
}*/

