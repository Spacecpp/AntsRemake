#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

//#include "GameObject.h"

#include "Alegrinho.h"

class WorldObjectData;


class WorldObject//: public GameObject
{
	
	protected:
		WorldObjectData * myData;
		
		virtual void init();
		virtual bool link_data();
	
	public:
		bool active;
		IntVector2 position;
		
		uint8_t type;
		//uint8_t pivot;
		uint8_t animationFrame;
		uint8_t animationDelay;
		uint8_t variant;
		
		
		WorldObject();
		WorldObject(int x, int y, uint8_t type, uint8_t variant = 0);
		//WorldObject(float x, float y, char type);
		
		void destroy();

		int get_total_animation_frames() const;
		int get_animation_delay() const;
		int get_variants_count() const;
		
		
		float get_left() const;
		float get_top() const;
		
		float get_width() const;
		float get_height() const;
		
		float get_center_horizontal() const;
		float get_center_vertical() const;
		Vector2 get_center_position() const;
		
		int get_tile_width() const;
		int get_pivot() const;
		
		bool tile_collision(int x, int y) const;
		IntVector2 get_tile_position() const;
		
		Vector2 get_draw_position(float sprite_width, float sprite_height) const;
		
		uint8_t get_variant_index() const;
		
		void update();
		//void draw() const;
		
		void debug_printf();
		void debug_draw_collision_box(float line_thickness = 1.0f);
		
};
#endif


