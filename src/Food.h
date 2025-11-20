#ifndef FOOD_H
#define FOOD_H

#include "WorldObject.h"

//class FoodData;


class Food: public WorldObject
{
	
	protected:
		//FoodData * myData;
		
		void init();
		bool link_data();
	
	public:
		//uint8_t type;
		int points;
		
		/*
		uint8_t animationFrame;
		uint8_t animationDelay;
		uint8_t variant;
		*/
		
		Food();
		Food(int x, int y, uint8_t type, uint8_t variant = 0);
		//Food(float x, float y, char type);
		
		int get_points_per_piece() const;
		int get_total_pieces() const;
		ColorRGBA get_piece_color() const;
		int get_total_eat_frames() const;
		
		/*
		int get_total_animation_frames() const;
		int get_animation_delay() const;
		int get_variants_count() const;
		
		float get_width() const;
		int get_tile_width() const;
		bool tile_collision(int x, int y) const;
		*/
		
		int get_current_eat_frame() const;
		
		//void update();
		void draw() const;
		
		// net
		void pack_resync_data(BitStream & data);
		void unpack_resync_data(BitStream & _data);
		
		//void debug_printf();
		
};
#endif
