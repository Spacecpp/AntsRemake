#ifndef WORLDOBJECTDATA_H
#define WORLDOBJECTDATA_H

#include "Alegrinho.h"
#include <string>

class FileParser;


#define PIVOT_H_CENTER 0
#define PIVOT_V_CENTER 0
#define PIVOT_LEFT     1 << 0
#define PIVOT_RIGHT    1 << 1
#define PIVOT_TOP      1 << 2
#define PIVOT_BOTTOM   1 << 3


class WorldObjectData
{
	
	public:
		int id;
		uint8_t pivot;
		float radius, width, height;
		float draw_offset_x, draw_offset_y;
		uint8_t animationFrames;
		uint16_t animationDelay;
		uint8_t variants;
		std::string name;
		Bitmap sprite;
		Bitmap firstFrameSprite;
		
		WorldObjectData();
		~WorldObjectData();
		
		bool load(FileParser & parser);
		virtual bool parse_line(FileParser & parser);
		//bool load_sprite();
		
		void debug_printf();
		virtual bool update_first_frame_sprite();
		Bitmap * get_first_frame_sprite();

};
#endif

