#include "AntHill.h"
#include "globals.h"
#include "Player.h"
#include "Alegrinho.h"
#include "GameManager.h"


AntHill::AntHill(): GameObject(), /*foodPoints(0),*/ eggHatchTime(0), owner(NULL), antLocked(false), flags(0) //, nearbyAnts(0)
{
	
}



AntHill::AntHill(int x, int y, Player * player, uint8_t flags): GameObject(x, y), /*foodPoints(0),*/ eggHatchTime(0), owner(player), antLocked(false), flags(flags) //, nearbyAnts(0)
{
	
	if (!is_bit_flag_set(flags, ANTHILL_SYMMETRICAL) )
		position = Vector2(x * TILE_SIZE, y * TILE_SIZE);
		
}


void AntHill::reset()
{
	
	//foodPoints = 0; 
	eggHatchTime = 0;
	
}


void AntHill::update()
{
	
	int ants_over = count_ants_over_me();
	
	if (eggHatchTime > 1)
		eggHatchTime--;
	
	if (eggHatchTime == 1) // formiga pendente
	{
		
		// cria formiga se o local estiver desocupado
		IntVector2 tile_position = get_tile_position();
		//if (!gameManager->tile_occupied(tile_position.x, tile_position.y) )
		if (ants_over <= 0)
		{
			
			if (Ant * new_ant = gameManager->create_Ant( Ant(tile_position.x, tile_position.y, owner, gameManager->antNextUniqueID++) ) )
			{
				
				// todo: tirar lock de outra formiga?
				new_ant->IHaveTheAnthillLock = true;
				new_ant->actionDelay = 20;
				new_ant->currentPowerup = gameManager->gameMap.defaultPowerup;
				new_ant->move_away();
				
				antLocked = true;
				ants_over++;
				
				if (owner)
					owner->hatches++;
				
				play_sound(s_newAnt);
				
			}
			
			eggHatchTime = 0; // 0 significa desocupado
			
		}
		
	}
	
	/*
	// destrava naturalmente
	if (antLocked && ants_over <= 0)
		antLocked = false;
	*/
	
	//antLocked = false;
	//calculate_nearby_ants();
	
}



void AntHill::check_auto_unlock()
{
	
	if (antLocked)
	{
		
		for (Ant & c_Ant : gameManager->ant) 
		{
			
			if (c_Ant.IHaveTheAnthillLock)
				return;
			
			/*
			if (c_Ant.active && c_Ant.owner == this->owner)
			{
				
				if (ant_over_me(c_Ant) )
					return;
				
			}*/
			
		}
		
		antLocked = false;
		
	}
	
}



/*
void AntHill::calculate_nearby_ants()
{
	
	nearbyAnts = 0;
	
	for (Ant & c_Ant : gameManager->ant) 
	{
		
		if (c_Ant.active)
		{
			
			
			// const float distance_2 = this->distance_to_squared(c_Ant);
			// const float collision_distance_2 = (ANTHILL_RADIUS + ANT_RADIUS) * (ANTHILL_RADIUS + ANT_RADIUS);
			
			// if (distance_2 <= collision_distance_2)
				// nearbyAnts++;
			
			
			if (ant_over_me(c_Ant) )
				nearbyAnts++;
			
		}
		
	}
	
}*/



int AntHill::count_ants_over_me()
{
	
	int count = 0;
	
	for (Ant & c_Ant : gameManager->ant) 
	{
		
		if (c_Ant.active && c_Ant.owner == this->owner)
		{
			
			if (ant_over_me(c_Ant) )
				count++;
			
		}
		
	}
	
	return count;

}



bool AntHill::can_hatch_egg() const
{
	return (owner && eggHatchTime <= 0 && owner->foodPoints >= EGG_HATCH_COST);
}



void AntHill::hatch_egg()
{
	
	if (can_hatch_egg() )
	{
		
		eggHatchTime = 120;
		owner->foodPoints -= EGG_HATCH_COST;
		gameManager->create_FlyingText( FlyingText(this->position.x, this->position.y, "-" + to_stringi(EGG_HATCH_COST), true) );
		play_sound(s_hatch);
		
	}
	else printf("Egg hatch aborted\n");
	
}



IntVector2 AntHill::get_hole_tile_position() const
{
	
	IntVector2 offset(0, 0);
	
	if (!is_bit_flag_set(flags, ANTHILL_SYMMETRICAL) )
	{
		
		offset.x = is_bit_flag_set(flags, ANTHILL_FLIP_HORIZONTAL) ? 2 : 1;
		offset.y = is_bit_flag_set(flags, ANTHILL_FLIP_VERTICAL) ? 2 : 1;
		
	}
	
	return IntVector2( (int)(position.x / (float)TILE_SIZE) + offset.x, (int)(position.y / (float)TILE_SIZE) + offset.y);
	
}



Vector2 AntHill::get_hole_position() const
{
	
	IntVector2 pos = get_hole_tile_position();
	return Vector2(pos.x * TILE_SIZE + TILE_SIZE * 0.5f, pos.y * TILE_SIZE + TILE_SIZE * 0.5f);
}



IntVector2 AntHill::get_steal_hole_tile_position() const
{
	
	if (is_symmetrical() )
		return get_hole_tile_position();
	
	
	IntVector2 offset(0, 0);
	
	offset.x = is_bit_flag_set(flags, ANTHILL_FLIP_HORIZONTAL) ? 0 : 3;
	offset.y = is_bit_flag_set(flags, ANTHILL_FLIP_VERTICAL) ? 1 : 2;
	
	return IntVector2( (int)(position.x / (float)TILE_SIZE) + offset.x, (int)(position.y / (float)TILE_SIZE) + offset.y);
	
}



void AntHill::draw() const
{
	
	Bitmap * sprite = NULL;
	if (is_bit_flag_set(flags, ANTHILL_SYMMETRICAL) )
		sprite = &antHillSymetricSprite; 
		
	else
	{
		
		int colorIndex = 0;
		if (owner)
			colorIndex = owner->colorIndex;
			
		if (colorIndex < 0 || colorIndex >= 4) // temp
			colorIndex = 0;
			
		if (colorIndex >= 0 && colorIndex < 4)
			sprite = &antHillDefaultSprite[colorIndex];
			
	}
	if (!sprite) return;
	
	
	const float src_width = sprite->get_width();
	const float src_height = sprite->get_height();
	//const float half_width = src_width * 0.5f;
	//const float half_height = src_height * 0.5f;
	const float draw_width = src_width;
    const float draw_height = src_height;
    
    int draw_flags = 0;
    bool centered = false;
    
    if (is_bit_flag_set(flags, ANTHILL_SYMMETRICAL) )
		centered = true;
		
	else
    {
    	
		if (is_bit_flag_set(flags, ANTHILL_FLIP_HORIZONTAL) )
			draw_flags |= BITMAP_FLIP_HORIZONTAL;
			
		if (is_bit_flag_set(flags, ANTHILL_FLIP_VERTICAL) )
			draw_flags |= BITMAP_FLIP_VERTICAL;
			
    }
	
	const float draw_x = position.x - (centered ? draw_width * 0.5f : 0.0f); //(float)this->x * TILE_SIZE + TILE_SIZE * 0.5f;
	const float draw_y = position.y - (centered ? draw_height * 0.5f: 0.0f); //(float)this->y * TILE_SIZE + TILE_SIZE * 0.5f;
	
	ColorRGBA color = (show_debug && antLocked) ? RED : WHITE;
	
	//DrawTexturePro(*sprite, Rectangle { 0.0f, 0.0f, src_width, src_height }, Rectangle { draw_x, draw_y, src_width, src_height }, Vector2 { half_width, half_height }, 0.0f, color);
	
	spriteCache.draw_tinted_sprite_region(*sprite, color, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height, draw_flags);
	
}



void AntHill::draw_flag() const
{
	
	if ( !is_bit_flag_set(flags, ANTHILL_SYMMETRICAL) ) return;
	
	Bitmap * sprite = &antHillFlagSprite;
	
	const float src_width = sprite->get_width() / FLAG_TOTAL_ANIMATION_FRAMES;
	const float src_height = sprite->get_height();
	
	const float draw_width = src_width;
    const float draw_height = src_height;
    
    bool centered = is_bit_flag_set(flags, ANTHILL_SYMMETRICAL);
	
	const float draw_x = position.x + TILE_SIZE * 0.65f - draw_width  * 0.5f + (centered ? 0.0f : TILE_SIZE * 3);
	const float draw_y = position.y + TILE_SIZE * 0.5f  - draw_height * 0.5f + (centered ? 0.0f : TILE_SIZE * 3);
	
	const float src_x = (float)gameManager->flagAnimationFrame * src_width;
	
	spriteCache.draw_tinted_sprite_region(*sprite, get_my_Color(), src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
	
}


ColorRGBA AntHill::get_my_Color() const
{
	//return (owner ? owner->color : WHITE );
	return (owner ? owner->get_bright_color() : WHITE);
}



float AntHill::get_radius() const
{
	return ANTHILL_RADIUS;
}



bool AntHill::tile_over_me(int x, int y) const
{
	
	// considera que o formigueiro ocupa 3x3 tiles
	
	IntVector2 myTilePosition = get_tile_position();
	
	if (x < myTilePosition.x - 1) return false;
	if (x > myTilePosition.x + 1) return false;
	if (y < myTilePosition.y - 1) return false;
	if (y > myTilePosition.y + 1) return false;
	return true;
	
}



bool AntHill::ant_over_me(Ant & ant) const
{
	
	IntVector2 antTile = ant.get_tile_position();
	return tile_over_me(antTile.x, antTile.y);
	
}


bool AntHill::is_symmetrical() const
{ 
	return is_bit_flag_set(flags, ANTHILL_SYMMETRICAL);
}

