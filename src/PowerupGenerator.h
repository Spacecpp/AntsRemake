#ifndef POWERUPGENERATOR_H
#define POWERUPGENERATOR_H

#include "Alegrinho.h"

class PowerupGenerator
{
	
	public:
		bool active;
		IntVector2 position;
		uint32_t time, totalTime;
		bool dropping;
		uint16_t dropTime;
		uint8_t itemBan;
		
		
		PowerupGenerator();
		PowerupGenerator(int x, int y, uint32_t time, uint8_t bans);
		
		void destroy();
		
		void update();
		void spawn_item();
		
		void draw_editor_indicator() const;
		void draw_editor_time() const;
		void draw_droplet() const;
		
		void debug_printf();
		void debug_draw_collision_box(float line_thickness = 1.0f);
		
};
#endif
