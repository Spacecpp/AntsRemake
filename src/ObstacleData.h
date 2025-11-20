#ifndef OBSTACLEDATA_H
#define OBSTACLEDATA_H

#include "Alegrinho.h"
#include "WorldObjectData.h"

class FileParser;


class ObstacleData: public WorldObjectData
{
	
	public:
		//int id;
		//float radius;
		//uint8_t animationFrames;
		//uint16_t animationDelay;
		//uint8_t variants;
		//std::string name;
		//Bitmap sprite;
		
		ObstacleData();
		~ObstacleData();
		
		//bool load(FileParser & parser);
		bool parse_line(FileParser & parser);
		bool load_sprite();
		
		//void debug_printf();

};
#endif
