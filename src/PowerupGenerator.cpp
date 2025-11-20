#include "PowerupGenerator.h"
#include "lists.h"
#include "globals.h"
#include "Powerup.h"
#include "GameManager.h"

PowerupGenerator::PowerupGenerator(): active(false), position(0, 0), time(0), totalTime(time), dropping(false), dropTime(0), itemBan(0)
{
	
}

PowerupGenerator::PowerupGenerator(int x, int y, uint32_t time, uint8_t bans): active(true), position(x, y), time(time), totalTime(time), dropping(false), dropTime(0), itemBan(bans)
{
	
}



void PowerupGenerator::destroy()
{
	active = false;
}



void PowerupGenerator::update()
{
	
	if (time > 0)
		time--;
		
	if (time <= 0)
	{
		
		time = totalTime;
		
		// spawn the droplet
		dropping = true;
		dropTime = global_powerupDropAnimationFrames * global_powerupDropAnimationDelay;
		
		gameManager->play_area_sound(s_powerupDrop, position.x, position.y);
		
	}
	
	if (dropping)
	{
		
		if (dropTime > 0)
			dropTime--;
		
		if (dropTime <= 0)
		{
			
			dropping = false;
			spawn_item();
			
		}
		
	}
	
}



void PowerupGenerator::spawn_item()
{
	
	// TODO: aplicar bans
	//const int type = iRand(POWERUP::NONE + 1, POWERUP::COUNT - 1); // mudar pra usar seed do gamemanager
	const int type = gameManager->gameRNG.random_int(POWERUP::NONE + 1, POWERUP::COUNT - 1);
	
	if (Powerup * present_powerup = gameManager->get_powerup_at_tile(position.x, position.y) )
		present_powerup->type = type;
	
	else
		gameManager->create_Powerup( Powerup(position.x, position.y, type) );
	
}



void PowerupGenerator::draw_editor_indicator() const
{
	
	const float draw_x = position.x * TILE_SIZE;
	const float draw_y = position.y * TILE_SIZE;
	spriteCache.draw_tinted_sprite_region(whiteSquareBitmap, ColorRGBA(0.5f, 0.5f, 0.0f, 0.5f), 0.0f, 0.0f, whiteSquareBitmap.get_width(), whiteSquareBitmap.get_height(), draw_x, draw_y, TILE_SIZE, TILE_SIZE);
	
}


void PowerupGenerator::draw_editor_time() const
{
	
	const float draw_x = position.x * TILE_SIZE + TILE_SIZE * 0.5f;
	const float draw_y = position.y * TILE_SIZE + TILE_SIZE * 0.5f;
	String str = to_stringi(totalTime / 60);
	spriteCache.draw_text_height(the_font, str, WHITE, draw_x, draw_y, TILE_SIZE, TEXT_ALIGN_CENTER | TEXT_VERTICAL_ALIGN_CENTER_BASELINE);
	
}



void PowerupGenerator::draw_droplet() const
{
	
	if (!dropping) return;
	
	const float base_width = powerupDropSprite.get_width();
	const float base_height = powerupDropSprite.get_height();
	
	const float src_width = global_powerupDropAnimationFrames > 0 ? base_width / (float)global_powerupDropAnimationFrames : base_width;
	const float src_height = base_height;
	
	const int current_frame = global_powerupDropAnimationDelay > 0 ? global_powerupDropAnimationFrames - (dropTime / global_powerupDropAnimationDelay) : 0;
	
	const float src_x = (float)current_frame * src_width;
	
	const float draw_width = src_width;
	const float draw_height = src_height;
	
	const float draw_x = position.x * TILE_SIZE + TILE_SIZE * 0.5f - draw_width * 0.5f;
	const float draw_y = position.y * TILE_SIZE + TILE_SIZE - draw_height;
	
	spriteCache.draw_sprite_region(powerupDropSprite, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
	
	
	//spriteCache.draw_sprite_region(powerupDropSprite, 0.0f, 0.0f, powerupDropSprite.get_width(), powerupDropSprite.get_height(), position.x * TILE_SIZE, position.y * TILE_SIZE, powerupDropSprite.get_width(), powerupDropSprite.get_height());
}



void PowerupGenerator::debug_printf()
{
	
	
}



void PowerupGenerator::debug_draw_collision_box(float line_thickness)
{
	
	
}

