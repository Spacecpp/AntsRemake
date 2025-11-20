#include "WorldObjectData.h"
#include "FileParser.h"


WorldObjectData::WorldObjectData(): id(0), pivot(0), radius(0.0f), width(0.0f), height(0.0f), draw_offset_x(0.0f), draw_offset_y(0.0f), animationFrames(1), animationDelay(0), variants(1)
{
	
}


WorldObjectData::~WorldObjectData()
{
	sprite.destroy();
}



bool WorldObjectData::load(FileParser & parser)
{
	
	while (!parser.end_of_file() && !parser.error() ) 
	{
		
		parser.get_next_line();
		
		/*
		std::string temp = parser.line;
		trim(temp);
		printf("%s\n", temp.c_str() );
		*/
		
		if (parser.key_and_value_ok() )
			parse_line(parser);
		
		else if (parser.empty_line() )
			break;
		
		else if (!parser.key_missing_value() )
			printf("%d: Parse error: %s\n", parser.linenumber, parser.line.c_str() );
		
	}
	
	return true;
	
}


bool WorldObjectData::parse_line(FileParser & parser)
{
	
	bool ok = false;
	
	std::string key = string_to_lowercase(parser.key);
	std::string value = string_to_lowercase(parser.value);
	//bool bad_value = false;
	
	//printf("%s = %s\n", key.c_str(), value.c_str() );
	
	if (key == "name")
	{
		this->name = parser.value; // manter case
		ok = true;
	}
	
	else if (key == "id")
	{
		this->id = string_to_int(value);
		ok = true;
	}
	
	else if (key == "radius")
	{
		this->radius = string_to_float(value);
		ok = true;
	}
	
	else if (key == "width")
	{
		this->width = string_to_float(value);
		ok = true;
	}
	
	else if (key == "height")
	{
		this->height = string_to_float(value);
		ok = true;
	}
	
	else if (key == "animationframes")
	{
		this->animationFrames = string_to_int(value);
		ok = true;
	}
	
	else if (key == "animationdelay")
	{
		this->animationDelay = string_to_int(value);
		ok = true;
	}
	
	else if (key == "variants")
	{
		this->variants = string_to_int(value);
		ok = true;
	}
	
	else if (key == "pivot")
	{
		
		int8_t new_pivot = 0;
		
		if (value.find("left") != String::npos)
		{
			set_bit_flag(new_pivot, PIVOT_LEFT);
			ok = true;
		}
		
		else if (value.find("right") != String::npos)
		{
			set_bit_flag(new_pivot, PIVOT_RIGHT);
			ok = true;
		}
		
		if (value.find("top") != String::npos)
		{
			set_bit_flag(new_pivot, PIVOT_TOP);
			ok = true;
		}
		
		else if (value.find("bottom") != String::npos)
		{
			set_bit_flag(new_pivot, PIVOT_BOTTOM);
			ok = true;
		}
		
		if (ok)
			this->pivot = new_pivot;
		else
			ok = true; // confirma que reconheceu a key
		
	}
	
	else if (key == "drawoffsetx")
	{
		this->draw_offset_x = string_to_float(value);
		ok = true;
	}
	
	else if (key == "drawoffsety")
	{
		this->draw_offset_y = string_to_float(value);
		ok = true;
	}
	
	
	return ok;
	
}


/*
bool WorldObjectData::load_sprite()
{
	
	std::string filepath = "Data/Graphics/Food/" + this->name + ".png";
	return sprite.load(filepath);
	
}*/

/*
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
*/

void WorldObjectData::debug_printf()
{
	
	printf("id: %d, name: %s, pivot: %d\n", id, name.c_str(), (unsigned int)pivot);
	
}


bool WorldObjectData::update_first_frame_sprite()
{
	
	firstFrameSprite.destroy();
	if (sprite.is_ready() )
	{
		const float w = sprite.get_width() / std::max( (float)variants, 1.0f);
		const float h = sprite.get_height() / std::max( (float)animationFrames, 1.0f);
		return firstFrameSprite.create_sub(sprite, 0, 0, w, h);
	}
	return false;
	
}


Bitmap * WorldObjectData::get_first_frame_sprite()
{
	
	if (firstFrameSprite.is_ready() )
		return &firstFrameSprite;
	
	if (sprite.is_ready() )
		return &sprite;
	
	return NULL;
	
}
