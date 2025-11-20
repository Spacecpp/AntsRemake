#include "ObstacleData.h"
#include "FileParser.h"


ObstacleData::ObstacleData(): WorldObjectData()
{
	
}


ObstacleData::~ObstacleData()
{
	sprite.destroy();
}




bool ObstacleData::parse_line(FileParser & parser)
{
	return WorldObjectData::parse_line(parser);
}



bool ObstacleData::load_sprite()
{
	
	std::string filepath = "Data/Graphics/Obstacles/" + this->name + ".png";
	if (sprite.load(filepath) )
	{
		update_first_frame_sprite();
		return true;
	}
	return false;
	
}
