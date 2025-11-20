#include "WorldObject.h"
#include "WorldObjectData.h"
#include "globals.h"
#include "lists.h"
#include "Alegrinho.h"
#include "GameManager.h"

/*
WorldObject::WorldObject(): GameObject(), myData(NULL), type(0), animationFrame(0), animationDelay(0), variant(0)
{
	
}

WorldObject::WorldObject(int x, int y, char type): GameObject(x, y), myData(NULL), type(type), animationFrame(0), animationDelay(0), variant(0)
{
	init();
}

WorldObject::WorldObject(float x, float y, char type): GameObject(x, y), myData(NULL), type(type), animationFrame(0), animationDelay(0), variant(0)
{
	init();
}*/



WorldObject::WorldObject(): myData(NULL), active(false), position(0, 0), type(0), animationFrame(0), animationDelay(0), variant(0)
{
	
}


WorldObject::WorldObject(int x, int y, uint8_t type, uint8_t variant): myData(NULL), active(true), position(x, y), type(type), animationFrame(0), animationDelay(0), variant(variant)
{
	init();
}



void WorldObject::destroy()
{
	active = false;
}



int WorldObject::get_total_animation_frames() const
{
	return (myData ? std::max( (int)myData->animationFrames, 1) : 1);
}


int WorldObject::get_animation_delay() const
{
	return (myData ? (int)myData->animationDelay : 0);
}


int WorldObject::get_variants_count() const
{
	return (myData ? std::max( (int)myData->variants, 1) : 1);
}




float WorldObject::get_left() const
{
	
	const int8_t pivot = get_pivot();
	
	if (is_bit_flag_set(pivot, PIVOT_LEFT) )
		return position.x * TILE_SIZE;
	
	if (is_bit_flag_set(pivot, PIVOT_RIGHT) )
		return (position.x + 1) * TILE_SIZE - get_width();
	
	// center
	return position.x * TILE_SIZE + TILE_SIZE / 2 - get_width() * 0.5f;
	
}


float WorldObject::get_top() const
{
	
	const int8_t pivot = get_pivot();
	
	if (is_bit_flag_set(pivot, PIVOT_TOP) )
		return position.y * TILE_SIZE;
	
	if (is_bit_flag_set(pivot, PIVOT_BOTTOM) )
		return (position.y + 1) * TILE_SIZE - get_height();
	
	return position.y * TILE_SIZE + TILE_SIZE / 2 - get_height() * 0.5f;
	
}



float WorldObject::get_width() const
{
	//return (float)get_tile_width() * (float)TILE_SIZE;
	//return (myData ? myData->radius * 2.0f : (float)TILE_SIZE * 2.0f);
	if (myData)
		return myData->width > 0.0f ? myData->width : myData->radius * 2.0f;
	return TILE_SIZE;
	
}


float WorldObject::get_height() const
{
	//return (float)get_tile_width() * (float)TILE_SIZE;
	//return (myData ? myData->radius * 2.0f : (float)TILE_SIZE * 2.0f);
	if (myData)
		return myData->height > 0.0f ? myData->height : myData->radius * 2.0f;
	return TILE_SIZE;
	
}


float WorldObject::get_center_horizontal() const
{
	return get_left() + get_width() * 0.5f;
}



float WorldObject::get_center_vertical() const
{
	return get_top() + get_height() * 0.5f;
}


Vector2 WorldObject::get_center_position() const
{
	return Vector2(get_center_horizontal(), get_center_vertical() );
}


int WorldObject::get_tile_width() const
{
	return (int)(get_width() / (float)TILE_SIZE);
}


int WorldObject::get_pivot() const
{
	return myData ? myData->pivot : 0;
}


bool WorldObject::tile_collision(int x, int y) const
{
    /*
    const int width = get_tile_width();
    IntVector2 WorldObject_tile = get_tile_position();
    return point_to_rectangle_collision(x, y, WorldObject_tile.x, WorldObject_tile.y, width, width);
    */
    
    const float left = get_left();
    const float top = get_top();
    const float width = get_width();
    const float height = get_height();
    
    const int tile_left = left / TILE_SIZE;
    if (x < tile_left) return false;
    
    const int tile_top = top / TILE_SIZE;
    if (y < tile_top) return false;
    
    const int tile_right = (left + width) / TILE_SIZE;
    if (x > tile_right) return false;
    
    const int tile_bottom = (top + height) / TILE_SIZE;
    if (y > tile_bottom) return false;
    
    return true;
    
}


IntVector2 WorldObject::get_tile_position() const
{
	return position;
}



Vector2 WorldObject::get_draw_position(float sprite_width, float sprite_height) const
{
	
	
	/*
	const float left = get_left();
    const float top = get_top();
    const float width = get_width();
    const float height = get_height();
    
    const float draw_offset_x = myData ? myData->draw_offset_x : 0.0f;
    const float draw_offset_y = myData ? myData->draw_offset_y : 0.0f;
    
    return Vector2(left + (width - sprite_width) * 0.5f + draw_offset_x, top + (height - sprite_height) * 0.5f + draw_offset_y);
    */
    
    
    Vector2 draw_position(0, 0);
    const int8_t pivot = get_pivot();
	
	if (is_bit_flag_set(pivot, PIVOT_LEFT) )
		draw_position.x = position.x * TILE_SIZE;
	
	else if (is_bit_flag_set(pivot, PIVOT_RIGHT) )
		draw_position.x = (position.x + 1) * TILE_SIZE - sprite_width;
	
	else 
		draw_position.x = position.x * TILE_SIZE + TILE_SIZE / 2 - sprite_width * 0.5f;
	
	
	if (is_bit_flag_set(pivot, PIVOT_TOP) )
		draw_position.y = position.y * TILE_SIZE;
	
	else if (is_bit_flag_set(pivot, PIVOT_BOTTOM) )
		draw_position.y = (position.y + 1) * TILE_SIZE - sprite_height;
	
	else 
		draw_position.y = position.y * TILE_SIZE + TILE_SIZE / 2 - sprite_height * 0.5f;
	
    
    if (myData)
    {
    	draw_position.x += myData->draw_offset_x;
    	draw_position.y += myData->draw_offset_y;
    }
    
	return draw_position;
	
}


uint8_t WorldObject::get_variant_index() const
{
	return variant > 0 ? variant - 1 : 1;
}


void WorldObject::update()
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
	
}




// private
void WorldObject::init()
{
	
	if (link_data() )
	{
		
		if (variant == 0)
		{
			const int variants_count = get_variants_count();
			variant = iRand(0, variants_count - 1) + 1;
		}
		
	}
	
}


//virtual
bool WorldObject::link_data()
{
	return false;
}




void WorldObject::debug_printf()
{
    
    //GameObject::debug_printf();
    printf("Position: %d, %d\n", position.x, position.y); 
    printf("get_width(): %.2f, get_tile_width(): %d\n", get_width(), get_tile_width() );
    printf("Pivot: %d\n", get_pivot() ); 
    
}



void WorldObject::debug_draw_collision_box(float line_thickness)
{
	
	const float x1 = get_left();
	const float y1 = get_top();
	
	const float x2 = x1 + get_width();
	const float y2 = y1 + get_height();
	
	spriteCache.draw_rectangle( x1, y1, x2, y2, RED, line_thickness);
	
	const float dist = 2;
	const float center_x = position.x * TILE_SIZE + TILE_SIZE / 2;
	const float center_y = position.y * TILE_SIZE + TILE_SIZE / 2;
	
	spriteCache.draw_rectangle( center_x - dist, center_y - dist, center_x + dist, center_y + dist, BLUE, line_thickness);
	
}
