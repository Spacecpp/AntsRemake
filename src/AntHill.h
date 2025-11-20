#ifndef ANTHILL_H
#define ANTHILL_H

#include "GameObject.h"
#include <stdint.h>

#define EGG_HATCH_COST 200

#define ANTHILL_NORMAL          0
#define ANTHILL_FLIP_HORIZONTAL 1 << 0
#define ANTHILL_FLIP_VERTICAL   1 << 1
#define ANTHILL_SYMMETRICAL     1 << 2

class Player;
class Ant;


class AntHill: public GameObject
{
	
	public:
		//int foodPoints;
		int8_t eggHatchTime;
		Player * owner;
		bool antLocked;
		uint8_t flags;
		//int8_t nearbyAnts;
		
		AntHill();
		AntHill(int x, int y, Player * player, uint8_t flags);
		
		void reset();
		
		void update();
		void check_auto_unlock();
		//void calculate_nearby_ants();
		int count_ants_over_me();
		
		bool can_hatch_egg() const;
		void hatch_egg();
		
		IntVector2 get_hole_tile_position() const;
		Vector2 get_hole_position() const;
		IntVector2 get_steal_hole_tile_position() const;
		
		void draw() const;
		void draw_flag() const;
		
		ColorRGBA get_my_Color() const;
		
		float get_radius() const;
		
		bool tile_over_me(int x, int y) const;
		bool ant_over_me(Ant & ant) const;
		
		bool is_symmetrical() const;
		
};
#endif
