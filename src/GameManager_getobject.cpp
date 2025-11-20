#include "GameManager.h"


Player * GameManager::find_local_player()
{
	
	for (Player & c_player : player) 
	{
		
		if (c_player.active && c_player.controller == CONTROLLER::LOCAL)
		{
			localPlayer = &c_player;
			return localPlayer;
		}
		
	}
	return NULL;
	
}



Ant * GameManager::get_Ant(int16_t unique_id)
{
	
	for (Ant & c_Ant : ant) 
	{
		
		if (c_Ant.active && c_Ant.unique_id == unique_id)
			return &c_Ant;
		
	}
	return NULL;
	
}



Ant * GameManager::get_Ant_at_position(float x, float y)
{
	
	for (Ant & c_Ant : ant) 
	{
		if (c_Ant.active)
		{
			
			if (point_on_GameObject(x, y, c_Ant) )
				return &c_Ant;
			
		}
	}
	return NULL;
	
}



std::vector<Ant*> GameManager::get_player_ants(Player & targetPlayer)
{
	
	std::vector<Ant*> ants;
	
	for (Ant & c_Ant : ant) 
	{
		if (c_Ant.active && c_Ant.owner == &targetPlayer)
			ants.push_back(&c_Ant);
	}
	
	return ants;
	
}



void GameManager::get_Ants_by_ids(std::vector<Ant*> & container, std::vector<int16_t> & ids)
{
	
	for (int16_t & id : ids)
	{
		
		for (Ant & c_Ant : ant) 
		{
			
			if (c_Ant.active && c_Ant.unique_id == id)
			{
				container.push_back(&c_Ant);
				break;
			}
			
		}
		
	}
	
}



Ant * GameManager::get_closest_enemy_ant(Ant & referenceAnt)
{
	
	Ant * bestTarget = NULL;
	float bestDistance = 0;
	
	for (Ant & c_Ant : ant) 
	{
		
		if (c_Ant.active && referenceAnt.is_enemy(c_Ant) )
		{
			
			const float distance = referenceAnt.distance_to_squared(c_Ant);
			if (!bestTarget || distance < bestDistance)
			{
				bestTarget = &c_Ant;
				bestDistance = distance;
			}
			
		}
		
	}
	
	return bestTarget;
	
}



Food * GameManager::get_food_at_tile(int x, int y)
{
	
	for (Food & c_food : food) 
	{
		
		if (c_food.active)
		{
			if (c_food.tile_collision(x, y) )
				return &c_food;
		}
		
	}
	
	return NULL;
	
}



Food * GameManager::get_food_at_radius(float x, float y, float radius)
{
	
	for (Food & c_food : food) 
	{
		
		if (c_food.active)
		{
			
			/*
			float distance_pow2 = distance_between_points_squared(x, y, c_food.position.x, c_food.position.y);
			
			const float width = c_food.get_width();
			const float half_width = width * 0.5f;
			
			const float radius2 = radius + half_width;
			
			if (distance_pow2 <= radius2 * radius2)
				return &c_food;
			*/
			
			const float left = c_food.get_left() - radius;
			const float top = c_food.get_top() - radius;
			const float width = c_food.get_width() + radius;
			const float height = c_food.get_height() + radius;
			
			if (point_to_rectangle_collision_topleft(x, y, left, top, width, height) )
				return &c_food;
			
		}
		
	}
	
	return NULL;
	
}



Food * GameManager::get_food_at_point(float x, float y)
{
	
	for (Food & c_food : food) 
	{
		
		if (c_food.active)
		{
			
			const float left = c_food.get_left();
			const float top = c_food.get_top();
			const float width = c_food.get_width();
			const float height = c_food.get_height();
			
			if (point_to_rectangle_collision_topleft(x, y, left, top, width, height) )
				return &c_food;
			
		}
		
	}
	
	return NULL;
	
}



Food * GameManager::get_closest_food(int x, int y)
{
	
	const float fx = x * TILE_SIZE + TILE_SIZE * 0.5f;
	const float fy = y * TILE_SIZE + TILE_SIZE * 0.5f;
	
	Food * best_food = NULL;
	float best_distance = 0;
	
	for (Food & c_food : food) 
	{
		
		if (c_food.active)
		{
			
			Vector2 pos = c_food.get_center_position();
			
			//float distance_pow2 = distance_between_points_squared(fx, fy, c_food.position.x, c_food.position.y);
			float distance_pow2 = distance_between_points_squared(fx, fy, pos.x, pos.y);
			
			if (!best_food || distance_pow2 < best_distance)
			{
				best_food = &c_food;
				best_distance = distance_pow2;
			}
			
		}
		
	}
	
	return best_food;
	
}


Powerup * GameManager::get_powerup_at_tile(int x, int y)
{
	
	for (Powerup & c_powerup : powerup) 
	{
		
		if (c_powerup.active)
		{
			
			IntVector2 tile_position = c_powerup.get_tile_position();
			if (tile_position.x == x && tile_position.y == y)
				return &c_powerup;
		}
		
	}
	
	return NULL;
	
}



Powerup * GameManager::get_powerup_at_radius(float x, float y, float radius)
{
	
	const float radius2 = radius * radius;
	for (Powerup & c_powerup : powerup) 
	{
		
		if (c_powerup.active)
		{
			
			float distance_pow2 = distance_between_points_squared(x, y, c_powerup.position.x, c_powerup.position.y);
			
			/*
			const float width = c_powerup.get_width();
			const float half_width = width * 0.5f;
			
			const float radius2 = radius + half_width;*/
			
			if (distance_pow2 <= radius2)
				return &c_powerup;
			
		}
		
	}
	
	return NULL;
	
}


AntHill * GameManager::get_AntHill(Player & owner)
{
	
	for (AntHill & c_antHill : antHill) 
	{
		if (c_antHill.active && c_antHill.owner == &owner)
			return &c_antHill;
	}
	printf("get_AntHill cant find target for player #%d %s\n", owner.index, owner.name.c_str() );
	return NULL;
	
}


AntHill * GameManager::get_AntHill(Player * owner)
{
	//return owner ? get_AntHill(*owner) : NULL;
	
	if (owner)
		return get_AntHill(*owner);
	printf("get_AntHill called with NULL pointer\n");
	return NULL;
}



AntHill * GameManager::get_AntHill(int player_id)
{
	
	if (Player * target_player = get_Player_by_id(player_id) )
		return get_AntHill(target_player);
	
	return NULL;
	
}



AntHill * GameManager::get_AntHill_at_tile(int x, int y)
{
	
	for (AntHill & c_antHill : antHill) 
	{
		if (c_antHill.active)
		{
			
			IntVector2 tilePosition = c_antHill.get_tile_position();
			
			// pivot no centro, 3 tiles de comprimento
			if (c_antHill.is_symmetrical() )
			{
				if (x >= tilePosition.x - 1 && x <= tilePosition.x + 1 &&
					y >= tilePosition.y - 1 && y <= tilePosition.y + 1)
					return &c_antHill;
			}
			
			else // pivot top left, 4 tiles
			{
				if (x >= tilePosition.x && x <= tilePosition.x + 3 &&
					y >= tilePosition.y && y <= tilePosition.y + 3)
					return &c_antHill;
			}
			
		}
	}
	return NULL;
	
}



AntHill * GameManager::get_AntHill_at_radius(float x, float y, float radius)
{
	
	const float total_radius = radius + ANTHILL_RADIUS;
	const float total_radius2 = total_radius * total_radius;
	
	for (AntHill & c_antHill : antHill) 
	{
		if (c_antHill.active)
		{
			
			float distance_pow2 = distance_between_points_squared(x, y, c_antHill.position.x, c_antHill.position.y);
			if (distance_pow2 <= total_radius2)
				return &c_antHill;
			
		}
	}
	return NULL;
	
}



Player * GameManager::get_Player_by_id(int id)
{
	
	for (Player & c_Player : player) 
	{
		if (c_Player.active && c_Player.id == id)
			return &c_Player;
	}
	return NULL;
	
}



Player * GameManager::get_Player_by_connection_id(int connection_id)
{
	
	for (Player & c_Player : player) 
	{
		if (c_Player.active && c_Player.connection_id == connection_id)
			return &c_Player;
	}
	return NULL;
	
}



Player * GameManager::get_Player_by_index(int index)
{
	if (index >= 0 && index < (int)player.size() )
	{
		if (player[index].active)
			return &player[index];
	}
	return NULL;
}
