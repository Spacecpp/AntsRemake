#include "Powerup.h"
//#include "globals.h"
#include "Alegrinho.h"

extern Bitmap powerupSprite[];
extern RenderCache spriteCache;

Powerup::Powerup(): GameObject(), type(0)
{
	
}


Powerup::Powerup(int x, int y, int8_t type): GameObject(x, y), type(type)
{
	
}


//void update();

void Powerup::draw() const
{
	
	const int index = type - 1;
	if (index < 0 || index > POWERUP::COUNT - 2) return;
	
	Bitmap * sprite = &powerupSprite[index];
	
	//const float src_width = sprite->get_width();
	const float src_width = sprite->get_width();
	const float src_height = sprite->get_height();
	//const float half_width = src_width * 0.5f;
	//const float half_height = src_height * 0.5f;
	const float draw_width = src_width;
    const float draw_height = src_height;
    
	//const float src_x = (float)(type - 1) * src_width;
	
	const float draw_x = position.x - draw_width * 0.5f; 
	const float draw_y = position.y - draw_height * 0.5f; 
	
	//DrawTexturePro(*sprite, Rectangle { src_x, 0.0f, src_width, src_height }, Rectangle { draw_x, draw_y, src_width, src_height }, Vector2 { half_width, half_height }, 0.0f, WHITE);
	
	spriteCache.draw_sprite_region(*sprite, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
	
}


// net
void Powerup::pack_data(BitStream & data)
{
    
    GameObject::pack_data(data);
    data.addInt8(type);
    
}



void Powerup::unpack_data(BitStream & _data)
{
    
    GameObject::unpack_data(_data);
    type = _data.getInt8();
    
}

