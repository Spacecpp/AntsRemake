#include "GameObject.h"
#include "globals.h"
#include "lists.h"
#include "GameManager.h"


GameObject::GameObject(): active(false), position(0.0f, 0.0f)
{
}

GameObject::GameObject(float x, float y): active(true), position(x, y)
{
}

GameObject::GameObject(int x, int y): active(true), position( (float)(x * TILE_SIZE + TILE_SIZE / 2), (float)(y * TILE_SIZE + TILE_SIZE / 2) )
{
}


void GameObject::destroy()
{
	active = false;
}



IntVector2 GameObject::get_tile_position() const
{
	return IntVector2( (int)(position.x / (float)TILE_SIZE), (int)(position.y / (float)TILE_SIZE) );
	
}


void GameObject::center_at_tile()
{
	
	const float half_tile = TILE_SIZE * 0.5f;
	position.x = round_down(position.x / (float)TILE_SIZE) * TILE_SIZE + half_tile;
	position.y = round_down(position.y / (float)TILE_SIZE) * TILE_SIZE + half_tile;
	
}


int8_t GameObject::get_tile_type() const
{
	
	IntVector2 my_tile = get_tile_position();
	return gameManager->gameMap.get_tile_type(my_tile.x, my_tile.y);
}



Tile * GameObject::get_tile() const
{
	
	IntVector2 my_tile = get_tile_position();
	return gameManager->gameMap.get_tile(my_tile.x, my_tile.y);
}


/*
void GameObject::emit_sound(Sound sound)
{
	gameManager->play_area_sound(sound, position.x, position.y);
}*/


void GameObject::move_rad(float distance, float rad_angle)
{
	this->position.x += cos(rad_angle) * distance;
	this->position.y += sin(rad_angle) * distance;
}




float GameObject::distance_to_squared(GameObject & other)
{
	return distance_between_points_squared(this->position.x, this->position.y, other.position.x, other.position.y);
}


float GameObject::angle_to_rad(GameObject & other)
{
	return angle_between_points_radians(this->position.x, this->position.y, other.position.x, other.position.y);
}

float GameObject::angle_to(GameObject & other)
{
	return angle_between_points(this->position.x, this->position.y, other.position.x, other.position.y);
}


float GameObject::angle_to_rad(float x, float y)
{
	return angle_between_points_radians(this->position.x, this->position.y, x, y);
}


// net
void GameObject::pack_data(BitStream & data)
{
    
    data.addFloat(position.x);
    data.addFloat(position.y);
    
}


void GameObject::unpack_data(BitStream & _data)
{
    
    active = true;
    position.x = _data.getFloat();
    position.y = _data.getFloat();
    
}



void GameObject::debug_printf()
{
    
   printf("Position: %.2f, %.2f\n", position.x, position.y); 
   
}
