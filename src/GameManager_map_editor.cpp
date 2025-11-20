#include "GameManager.h"
#include "interface_list.h"
#include "net_commands_list.h"


void GameManager::convert_gameplay_objects_into_map_format()
{
	
	gameMap.reset_spawners();
	
	for (const AntHill & current : antHill)  
	{
		gameMap.antHillSpawn.push_back( AntHillSpawn(current.position.x / TILE_SIZE, current.position.y / TILE_SIZE, current.owner ? current.owner->index + 1 : 0, current.flags) );
	}
	
	for (const Ant & current : ant) 
	{
		gameMap.antSpawn.push_back( AntSpawn(current.position.x / TILE_SIZE, current.position.y / TILE_SIZE, current.owner ? current.owner->index + 1 : 0) );
	}
	
	for (const Food & current : food) 
	{
		gameMap.foodSpawn.push_back( FoodSpawn(current.position.x, current.position.y, current.type) );
	}
	
	for (const Powerup & current : powerup) 
	{
		gameMap.itemSpawn.push_back( ItemSpawn(current.position.x / TILE_SIZE, current.position.y / TILE_SIZE, current.type) );
	}
	
	for (const Obstacle & current : obstacle) 
	{
		gameMap.obstacleSpawn.push_back( ObstacleSpawn(current.position.x, current.position.y, current.type) );
	}
	
	for (const PowerupGenerator & current : powerupGenerator) 
	{
		gameMap.powerupGeneratorSpawn.push_back( PowerupGeneratorSpawn(current.position.x, current.position.y, current.totalTime, current.itemBan) );
	}
	
	
}


void GameManager::convert_map_objects_into_gameplay_format()
{
	
	antHill.clear();
	ant.clear();
	food.clear();
	obstacle.clear();
	powerup.clear();
	powerupGenerator.clear();
	
	for (const AntHillSpawn & current : gameMap.antHillSpawn) 
	{
		if (Player * current_player = get_Player_by_index(current.z - 1) )
			antHill.push_back( AntHill(current.x, current.y, current_player, current.w) );
		else
			printf("Player %d not found\n", current.z);
	}
	
	for (const AntSpawn & current : gameMap.antSpawn) 
	{
		if (Player * current_player = get_Player_by_index(current.z - 1) )
			create_Ant( Ant(current.x, current.y, current_player, antNextUniqueID++) );
		else
			printf("Player %d not found\n", current.z);
	}
	
	for (const FoodSpawn & current : gameMap.foodSpawn) 
	{
		food.push_back( Food(current.x, current.y, current.z) );
	}
	
	for (const ItemSpawn & current : gameMap.itemSpawn) 
	{
		powerup.push_back( Powerup(current.x, current.y, current.z) );
	}
	
	for (const ObstacleSpawn & current : gameMap.obstacleSpawn) 
	{
		obstacle.push_back( Obstacle(current.x, current.y, current.z) );
	}
	
	for (const PowerupGeneratorSpawn & current : gameMap.powerupGeneratorSpawn) 
	{
		powerupGenerator.push_back( PowerupGenerator(current.x, current.y, current.z, current.w) );
	}
	
}



void GameManager::resize_map_in_all_directions(int up, int down, int left, int right)
{
	
	gameMap.resize_in_all_directions(up, down, left, right);
	gameMap.limit_size(MAP_DIMENSION_LIMIT);
	gameMap.calculate_tile_variants();
	
	// ajusta as coordenadas dos things para se adaptar a mudanca
	if (up != 0 || left != 0)
	{
		
		const float horizontal_distance = left * TILE_SIZE;
		const float vertical_distance = up * TILE_SIZE;
		
		move_all_objects(left, up);
		//move_all_particles(horizontal_distance, vertical_distance);
		camera.move_distance_absolute(horizontal_distance, vertical_distance);
		
	}
	
	// se a camera cair fora do mapa, manda de volta pro centro
	if (gameMap.coordinates_out_of_bounds(camera.x / TILE_SIZE, camera.y / TILE_SIZE) )
		center_camera();
	
	remove_out_of_bounds_objects();
	
	gameMap.clear_cache();
	//Minimap.update_all();
	gameMap.wasModified = true;
	
	//deepwater_need_update = true;
	
	refresh_camera();
	
}



void GameManager::move_all_objects(int tiles_h, int tiles_v)
{
	
	IntVector2 distance(tiles_h, tiles_v);
	Vector2 fDistance(tiles_h * TILE_SIZE, tiles_v * TILE_SIZE);
	
	for (AntHill & current : antHill)  
	{
		current.position += fDistance;
	}
	
	for (Ant & current : ant) 
	{
		current.position += fDistance;
	}
	
	for (Food & current : food) 
	{
		current.position += distance;
	}
	
	for (Powerup & current : powerup) 
	{
		current.position += fDistance;
	}
	
	for (Obstacle & current : obstacle) 
	{
		current.position += distance;
	}
	
}



void GameManager::remove_out_of_bounds_objects()
{
	
	for (AntHill & current : antHill)  
	{
		if (gameMap.world_coordinates_out_of_bounds(current.position.x, current.position.y) )
			current.destroy();
	}
	
	for (Ant & current : ant) 
	{
		if (gameMap.world_coordinates_out_of_bounds(current.position.x, current.position.y) )
			current.destroy();
	}
	
	for (Food & current : food) 
	{
		if (gameMap.coordinates_out_of_bounds(current.position.x, current.position.y) )
			current.destroy();
	}
	
	for (Powerup & current : powerup) 
	{
		if (gameMap.world_coordinates_out_of_bounds(current.position.x, current.position.y) )
			current.destroy();
	}
	
	for (Obstacle & current : obstacle) 
	{
		if (gameMap.coordinates_out_of_bounds(current.position.x, current.position.y) )
			current.destroy();
	}
	
}

