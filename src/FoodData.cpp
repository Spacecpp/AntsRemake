#include "FoodData.h"
#include "FileParser.h"


FoodData::FoodData(): WorldObjectData(), points(0), parts(0), eatFrames(1), partColor(1.0f, 1.0f, 1.0f, 1.0f)
{
	
}


FoodData::~FoodData()
{
	sprite.destroy();
}



bool FoodData::parse_line(FileParser & parser)
{
	
	bool ok = false;
	
	if (WorldObjectData::parse_line(parser) )
		ok = true;
	
	else
	{
		
		std::string key = string_to_lowercase(parser.key);
		std::string value = string_to_lowercase(parser.value);
		//bool bad_value = false;
		
		if (key == "points")
		{
			this->points = string_to_int(value);
			ok = true;
		}
		
		else if (key == "parts")
		{
			this->parts = string_to_int(value);
			ok = true;
		}
		
		else if (key == "eatframes")
		{
			this->eatFrames = string_to_int(value);
			ok = true;
		}
		
		else if (key == "partcolor")
		{
			
			std::string str = value;
			bool success = false;
			
			size_t pos1 = str.find(' ');
			if (pos1 != std::string::npos)
			{
				
				size_t pos2 = str.find(' ', pos1 + 1);
				if (pos2 != std::string::npos)
				{
					
					unsigned char r = (unsigned char)glm::clamp( string_to_int( str.substr(0, pos1) ), 0, 255);
					unsigned char g = (unsigned char)glm::clamp( string_to_int( str.substr(pos1 + 1, pos2 - pos1 - 1) ), 0, 255);
					unsigned char b = (unsigned char)glm::clamp( string_to_int( str.substr(pos2 + 1) ), 0, 255);
					
					this->partColor = color_int_to_float(r, g, b, 255);
					success = true;
					
				}
				
			}
			
			if (!success)
				printf("Failed to parse color\n");
			
			ok = true;
			
		}
		
	}
	
	return ok;
	
}


bool FoodData::load_sprite()
{
	
	std::string filepath = "Data/Graphics/Food/" + this->name + ".png";
	if (sprite.load(filepath) )
	{
		update_first_frame_sprite();
		return true;
	}
	return false;
	
}


bool FoodData::update_first_frame_sprite()
{
	
	firstFrameSprite.destroy();
	if (sprite.is_ready() )
	{
		const float w = (sprite.get_width() / std::max( (float)variants, 1.0f) ) / std::max( (float)eatFrames, 1.0f);
		const float h = sprite.get_height() / std::max( (float)animationFrames, 1.0f);
		return firstFrameSprite.create_sub(sprite, 0, 0, w, h);
	}
	return false;
	
}
