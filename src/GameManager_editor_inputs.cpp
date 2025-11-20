#include "GameManager.h"
#include "interface_list.h"
#include "net_commands_list.h"


void GameManager::process_editor_inputs()
{
	
	if (_GUI_Window.active) return;
	if (mouse_over_UI() ) return;
	
	// pinta / adiciona objetos
	if (mouse_left_pressed)
	{
		
		Vector2 mouse_position = get_mouse_world_position(); 
		IntVector2 tile_position(mouse_position.x / TILE_SIZE, mouse_position.y / TILE_SIZE);
		
		if (!gameMap.coordinates_out_of_bounds(tile_position.x, tile_position.y) )
		{
			
			if (editorElementType == EDITOR_ELEMENT::TERRAIN)
			{
				gameMap.terrain[tile_position.y][tile_position.x].type = editorTileType;
				gameMap.calculate_tile_variants(tile_position.x - 1, tile_position.y - 1, tile_position.x + 1, tile_position.y + 1);
				gameMap.renderCache.clear_cache();
			}
			
			else if (editorElementType == EDITOR_ELEMENT::TERRAIN_OBJECT)
			{
				int new_type = editorTileType + (editorTileType == SECONDARY_TILE::LANDMINE_0 ? editorPlayerID : 0);
				gameMap.terrain[tile_position.y][tile_position.x].secondary = new_type;
			}
			
			// objetos
			else if (mouse_left_down && !tile_occupied_by_object(tile_position.x, tile_position.y) ) //!gameMap.tile_occupied_by_object(tile_position.x, tile_position.y) )
			{
				
				if (editorElementType == EDITOR_ELEMENT::ANTHILL)
				{
					//gameMap.antHillSpawn.push_back( IntVector3(tile_position.x, tile_position.y, editorTileType) );
					if (editorPlayerID >= 0 && editorPlayerID < MAX_PLAYERS)
						antHill.push_back( AntHill(tile_position.x, tile_position.y, get_Player_by_index(editorPlayerID), editorAntHillFlags) );
				}
				
				else if (editorElementType == EDITOR_ELEMENT::ANT)
				{
					//gameMap.antSpawn.push_back( IntVector3(tile_position.x, tile_position.y, editorTileType) );
					if (editorTileType >= 0 && editorTileType < POWERUP::COUNT)
						create_Ant( Ant(tile_position.x, tile_position.y, get_Player_by_index(editorPlayerID), 0, editorTileType) );
				}
				
				else if (editorElementType == EDITOR_ELEMENT::FOOD)
				{
					//gameMap.foodSpawn.push_back( IntVector3(tile_position.x, tile_position.y, editorTileType) );
					food.push_back( Food(tile_position.x, tile_position.y, editorTileType, editorVariant) );
				}
				
				else if (editorElementType == EDITOR_ELEMENT::POWERUP)
				{
					//gameMap.itemSpawn.push_back( IntVector3(tile_position.x, tile_position.y, editorTileType) );
					if (editorTileType >= 0 && editorTileType < POWERUP::COUNT)
						powerup.push_back( Powerup(tile_position.x, tile_position.y, editorTileType) );
				}
				
				else if (editorElementType == EDITOR_ELEMENT::OBSTACLE)
				{
					//gameMap.obstacleSpawn.push_back( IntVector3(tile_position.x, tile_position.y, editorTileType) );
					obstacle.push_back( Obstacle(tile_position.x, tile_position.y, editorTileType, editorVariant) );
					calculate_solid_tiles();
				}
				
				else if (editorElementType == EDITOR_ELEMENT::POWERUP_GENERATOR)
				{
					powerupGenerator.push_back( PowerupGenerator(tile_position.x, tile_position.y, editorPowerupGenTime * 60, 0) );
				}
				
			}
			
			
		}
		
	}
	
	// erase
	if (mouse_right_pressed)
	{
		
		Vector2 mouse_position = get_mouse_world_position(); 
		IntVector2 tile_position(mouse_position.x / TILE_SIZE, mouse_position.y / TILE_SIZE);
		
		if (!gameMap.coordinates_out_of_bounds(tile_position.x, tile_position.y) )
		{
			
			if (editorElementType == EDITOR_ELEMENT::TERRAIN)
			{
				gameMap.terrain[tile_position.y][tile_position.x].type = 0;
				gameMap.calculate_tile_variants(tile_position.x - 1, tile_position.y - 1, tile_position.x + 1, tile_position.y + 1);
				gameMap.renderCache.clear_cache();
			}
			
			else if (editorElementType == EDITOR_ELEMENT::TERRAIN_OBJECT)
			{
				gameMap.terrain[tile_position.y][tile_position.x].secondary = 0;
			}
			
			else if (mouse_right_down) //if (editorElementType == EDITOR_ELEMENT::ANT)
				remove_object_at_tile(tile_position.x, tile_position.y);
			
		}
		
	}
	
	
	
	// escolha de variant
	if (key_down[KEY_1])
		editorVariant = 1;
	
	if (key_down[KEY_2])
		editorVariant = 2;
	
	if (key_down[KEY_3])
		editorVariant = 3;
	
	if (key_down[KEY_4])
		editorVariant = 4;
	
	if (key_down[KEY_5])
		editorVariant = 5;
	
	if (key_down[KEY_6])
		editorVariant = 6;
	
	if (key_down[KEY_7])
		editorVariant = 7;
		
    if (key_down[KEY_8])
		editorVariant = 8;
		
    if (key_down[KEY_9])
		editorVariant = 9;
	
	if (key_down[KEY_0])
		editorVariant = 0;
	
	if (key_down[KEY_KEYPAD_ADD])
	{
		if (editorVariant > 0)
			editorVariant++;
	}
	
	if (key_down[KEY_KEYPAD_SUBTRACT])
	{
		if (editorVariant < 9)
			editorVariant--;
	}
	
	/*
	// escolha de tile
	if (key_down[KEY_1])
		editorTileType = 0;
	
	if (key_down[KEY_2])
		editorTileType = 1;
	
	if (key_down[KEY_3])
		editorTileType = 2;
	
	if (key_down[KEY_4])
		editorTileType = 3;
	
	if (key_down[KEY_5])
		editorTileType = 4;
	
	if (key_down[KEY_6])
		editorTileType = 5;
	
	if (key_down[KEY_7])
		editorTileType = 6;
		
    if (key_down[KEY_8])
		editorTileType = 7;
		
    if (key_down[KEY_9])
		editorTileType = 8;
	
	if (key_down[KEY_KEYPAD_ADD])
		editorTileType++;
	
	if (key_down[KEY_KEYPAD_SUBTRACT])
		editorTileType--;
	*/
	
	/*
	// tipo de edicao
	if (key_down[KEY_Z])
		editorElementType = EDITOR_ELEMENT::TERRAIN;
	
	if (key_down[KEY_X])
		editorElementType = EDITOR_ELEMENT::ANTHILL;
	
	if (key_down[KEY_C])
		editorElementType = EDITOR_ELEMENT::ANT;
	
	if (key_down[KEY_V])
		editorElementType = EDITOR_ELEMENT::FOOD;
	
	if (key_down[KEY_B])
		editorElementType = EDITOR_ELEMENT::POWERUP;
	
	if (key_down[KEY_N])
		editorElementType = EDITOR_ELEMENT::OBSTACLE;
	*/
	
	/*
	if (!_GUI_Window.active)
	{
		
		// controle de tamanho do mapa
		int up = 0, down = 0, left = 0, right = 0;
		const int reverse_key = KEY_SPACE;
		
		if (key_down[KEY_KEYPAD_8])
		{
			up = (key_pressed[reverse_key] ? -1 : 1);
		}
		
		if (key_down[KEY_KEYPAD_2])
		{
			down = (key_pressed[reverse_key] ? -1 : 1);
		}
		
		if (key_down[KEY_KEYPAD_4])
		{
			left = (key_pressed[reverse_key] ? -1 : 1);
		}
		
		if (key_down[KEY_KEYPAD_6])
		{
			right = (key_pressed[reverse_key] ? -1 : 1);
		}
		
		if (up != 0 || down != 0 || left != 0 || right != 0)
		{
			
			gameMap.resize_in_all_directions(up, down, left, right);
			
			// compensa movimento da camera
			if (up != 0)
				camera.y += up * TILE_SIZE;
			
			if (left != 0)
				camera.x += left * TILE_SIZE;
				
		}
		
	}*/
	//---
	
	
	// save e load
	/*
	if (key_down[KEY_S])
	{
		
		String mappath = "Maps/" + selectedMapName + ".map";
		String result;
		if (save_map(mappath.c_str() ) )
			result = "Saved " + mappath;
		else
			result = "Error saving " + mappath;
			
		info_window(result);
		
	}
	
	if (key_down[KEY_L])
	{
		
		String mappath = "Maps/" + selectedMapName + ".map";
		String result;
		if (load_map(mappath.c_str() ) )
			result = "Loaded " + mappath;
		else
			result = "Error loading " + mappath;
		
		info_window(result);
		
	}*/
	
}
