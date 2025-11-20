#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Alegrinho.h"
#include <stdint.h>


class Tile;

class GameObject
{
	
	public:
		bool active;
		Vector2 position;
		
		GameObject();
		GameObject(float x, float y);
		GameObject(int x, int y);
		
		void destroy();
		
		IntVector2 get_tile_position() const;
		void center_at_tile();
		int8_t get_tile_type() const;
		Tile * get_tile() const;
		//void emit_sound(Sound sound);
		
		void move_rad(float distance, float rad_angle);
		
		float distance_to_squared(GameObject & other);
		
		float angle_to_rad(GameObject & other);
		float angle_to(GameObject & other);
		
		float angle_to_rad(float x, float y);
		
		// net
		void pack_data(BitStream & data);
		void unpack_data(BitStream & _data);
		
        void debug_printf();
        
};

#endif
