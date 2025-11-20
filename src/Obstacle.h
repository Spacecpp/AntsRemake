#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "WorldObject.h"

class ObstacleData;


class Obstacle: public WorldObject
{
	
	protected:
		//ObstacleData * myData;
		
		//void init();
		bool link_data();
	
	public:
		//uint8_t type;
		
		//uint8_t animationFrame;
		//uint8_t animationDelay;
		//uint8_t variantIndex;
		
		
		Obstacle();
		Obstacle(int x, int y, uint8_t type, uint8_t variant = 0);
		//Obstacle(float x, float y, char type);
		
		/*
		int get_total_animation_frames() const;
		int get_animation_delay() const;
		int get_variants_count() const;
		
		float get_width() const;
		int get_tile_width() const;
		bool tile_collision(int x, int y) const;
		*/
		
		//void update();
		void draw() const;
		
		//void debug_printf();
		
};
#endif

