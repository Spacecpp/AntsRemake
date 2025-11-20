#ifndef FOODDATA_H
#define FOODDATA_H

#include "Alegrinho.h"
#include "WorldObjectData.h"

class FileParser;


class FoodData: public WorldObjectData
{
	
	public:
		//int id;
		int points;
		int parts;
		//float radius;
		uint8_t eatFrames;
		//uint8_t animationFrames;
		//uint16_t animationDelay;
		//uint8_t variants;
		//std::string name;
		ColorRGBA partColor;
		//Bitmap sprite;
		
		FoodData();
		~FoodData();
		
		//bool load(FileParser & parser);
		bool parse_line(FileParser & parser);
		bool load_sprite();
		
		bool update_first_frame_sprite();
		
		//void debug_printf();


};
#endif


