#include "GameManager.h"
#include "interface_list.h"
#include "net_commands_list.h"

GameManager::GameManager()
{
	
	mapDragging = false;
	
	
	editMode = false;
	editorTileType = 0;
	editorElementType = 0;
	editorVariant = 0;
	editorPowerupGenTime = 0;
	editorAntHillFlags = 0;
	editorPlayerID = 0;
	editorPage = 0;
	
	joinIP = "127.0.0.1";
	port = DEFAULT_SERVER_PORT;
	localPlayerName = "Player";
	
	waterAnimationOffset = 0.0f;
	
	fireAnimationFrame = 0;
	fireAnimationDelay = 0;
	
	mineAnimationFrame = 0;
	mineAnimationDelay = 0;
	
	
	gameFrame = 0;
	commandDelay = 10;
	oldCommandDelay = commandDelay;
	networkFrame = 0; //commandDelay;
	gameFrameMax = 0; //networkFrame;
	
	gameTime = 0;
	gameInProgress = false;
	gameEnded = false;
	inGameSetupScreen = false;
	
	antNextUniqueID = 1;
	playerNextUniqueID = 1;
	
	//consoleOpen = false;
	
	draggingBoxSelection = false;
	
	localPlayer = NULL;
	
	pendingResync = false;
	
	debug_showPaths = false;
	simulateConnectionLost = false;
	
	menuAntAnimationFrame = 0;
	menuAntAnimationTime = 1;
	
	flagAnimationFrame = 0;
	flagAnimationTime = 1;
	
	edgeScrolling = false;
	musicDisabled = false;
	
	selectedMapName = "treasure";
	
	// load database
	load_food_data();
	load_obstacle_data();
	load_animation_data();
	
	
	// configura o console
	int console_width = 500;
	int console_height = 240;
	const bool borders_outside = true;
	const float slice_width = 4.0f;
	const float console_offset = 4.0f + (borders_outside ? slice_width : 0.0f);
	
	console.set(console_offset, screen_h - console_height - console_offset - 10.0f, console_width, console_height, true, 1.0f, 32);
	console.visible = false;
	console.sprite = &console9sliceSprite; //&window9sliceSprite;
	console._9slice_width = slice_width; 
	console._9slice_scale = 2.0f;
	console.font = &the_font;
	console.shadow_text = true;
	console.borders_outside = borders_outside;
	
	if (!load_config() )
	{
		localPlayerName = "Anter" + to_stringi( iRand(1000, 9999) );
	}
	
	enter_main_menu();
	
	//SetWindowOpacity(0.5f);  
	
}



void GameManager::reset()
{
	
	reset_gameplay();
	
	localPlayer = NULL;
	player.clear();
	
}



void GameManager::reset_gameplay()
{
	
	queuedOrder.clear();
	netQueuedOrder.clear();
	
	//consoleMessage.clear();
	//console.destroy();
	//consoleInput.clear();
	//consoleOpen = false;
	
	draggingBoxSelection = false;
	
	waterAnimationOffset = 0.0f;
	
	antNextUniqueID = 1;
	playerNextUniqueID = 1;
	gameEnded = false;
	
	/*
	gameFrame = 0;
	networkFrame = commandDelay;
	gameFrameMax = networkFrame;
	gameFrameDone = false;
	*/
	
	gameFrame = 0;
	networkFrame = 1;
	gameFrameMax = commandDelay - 1;
	gameFrameDone = false;
	
	
	pendingResync = false;
	
	selectedAnt.clear();
	
	antHill.clear();
	ant.clear();
	food.clear();
	obstacle.clear();
	powerup.clear();
	powerupGenerator.clear();
	
	foodChunk.clear();
	flyingText.clear();
	healParticle.clear();
	
}



void GameManager::main_loop()
{
	
	main_loop_should_run = true;
	while (main_loop_should_run)
	{
		
		events_update();
		
		while (should_run_a_frame() )
		{
			
			redraw = true;
			global_drawcall_count = 0;
			
			post_events_update();
			
			do_logic(); // ---
			
			refresh_inputs();
			
		}
		
		if (redraw && !halt_drawing)
		{
			
			redraw = false;
			draw(); // ---
			swap_buffers();
			
		}
		
	}
	
	shutdown();
	
}



void GameManager::shutdown()
{
	
	#ifdef ONLINE_MULTIPLAYER_AVAILABLE
	if (netController.is_online() )
		netController.disconnect();
	#endif
	
	save_config();
	
	// destrutores
	foodData.clear();
	
}



void GameManager::do_logic()
{
    
	if (gameState == GAMESTATE::EXIT)
		stop_main_loop();
	
	if (screen_resized)
	{
		refresh_camera();
		
		if (gameState == GAMESTATE::PLAYING) // o refresh no menu eh chamado em outro lugar
			place_console();
	}
	
	#ifdef ONLINE_MULTIPLAYER_AVAILABLE
	if (!simulateConnectionLost)
		Net_process_input();
	#endif
	
	if (gameState == GAMESTATE::PLAYING && !gameEnded)
        update_gameframe();
	
	// console
	// controle da visibilidade apertar enter durante gameplay
	/*
	if (gameState == GAMESTATE::PLAYING)
	{
		if (key_down[KEY_ENTER]|| key_down[KEY_KEYPAD_ENTER])
		{
			
			if (console.visible)
			{
				
				if (consoleInput.empty() ) // deixa sumir caso nao esteja digitando
				{

					//console.visible = false;
					//console.always_opaque = false;
					console.opacity = 1.0f;
					
				}
				
			}
			
			else  // deixa visivel
			{
				console.visible = true;
				console.inputFocus = true;
				console.always_opaque = true;
				console.opacity = 1.0f;
			}
			
		}
	}*/
	
	//if (console.active)
    {
		//console.update();
		
		int result = console.update();
		if (result > 0)
		{
			
			if (!is_client() )
				add_chat_message_to_the_console(console.inputText, localPlayer);
			
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
			if (netController.is_online() )
				communicate_chat_message(console.inputText, localPlayer);
			#endif
			
			console.inputText.clear();
			
		}
    }
    //----
    
	process_inputs();
	
	if (gameState == GAMESTATE::PLAYING)
		update_gameplay();
		
		
	// animacao de formiga no menu setup
	if (_GUI_Window.active && _GUI_Window.code == WINDOW_ID::GAME_SETUP)
	{
		
		menuAntAnimationTime--;
		if (menuAntAnimationTime <= 0)
		{
			
			menuAntAnimationTime = 10;
			
			menuAntAnimationFrame++;
			if (menuAntAnimationFrame >= ANT_ANIMATION_FRAMES)
				menuAntAnimationFrame = 0;
			
		}
	
	}
	
	
	// detecta mudancas que precisam ser sincronizadas
	for (Player & currentPlayer : player)
	{
		
		if (currentPlayer.active)
		{
			
			if (currentPlayer.oldColorIndex != currentPlayer.colorIndex)
			{
				currentPlayer.oldColorIndex = currentPlayer.colorIndex;
				
				#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				if (netController.is_online() && netController.any_connected() )
					comunicate_color_change(currentPlayer);
				#endif
				
			}
			
			if (currentPlayer.oldReady != currentPlayer.ready)
			{
				currentPlayer.oldReady = currentPlayer.ready;
				
				#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				if (netController.is_online() && netController.any_connected() )
					comunicate_ready_tick(currentPlayer);
				#endif
				
			}
			
		}
		
	}
	
	// detecta mudanca de commandDelay
	if (oldCommandDelay != commandDelay)
	{
		
		oldCommandDelay = commandDelay;
		
		#ifdef ONLINE_MULTIPLAYER_AVAILABLE
		if (netController.server_running() && !netController.server_is_empty() )
			comunicate_rule_update();
		#endif
		
	}
	
	#ifdef ONLINE_MULTIPLAYER_AVAILABLE
	if (!simulateConnectionLost)
        Net_process_output();
	#endif
	
	
	// controle de musica
	if (gameState == GAMESTATE::PLAYING && !musicDisabled)
	{
		if (music_ended() )
		{
			if (!play_random_ingame_music() )
				musicDisabled = true;
		}
	}
	
}



void GameManager::enter_main_menu()
{
	
	gameState = GAMESTATE::MENU;
	inGameSetupScreen = false;
	gameInProgress = false;
	editMode = false;
	
	gameMap.loaded = false; // forca recarregar na proxima
	
	//console.visible = false;
	console.destroy();
	mapEditor_upperMenu.destroy();
	mapEditor_toolWindow.destroy();
	
	main_menu_window();
	
	if (mus_mainMenu.is_ready() && !music_is_playing() )
		play_music(mus_mainMenu, true);
	
}



void GameManager::leave_game()
{
	
	if (gameState == GAMESTATE::PLAYING) // evitar de ficar recomecando ao sair do game setup
		stop_music();
		
	currentMusicFileName.clear();
	
	#ifdef ONLINE_MULTIPLAYER_AVAILABLE
	if (netController.is_online() )
		netController.disconnect();
	#endif
	
	enter_main_menu();
	
}



void GameManager::escape_command()
{
	
	if (gameState == GAMESTATE::MENU)
	{
		
		if (_GUI_Window.active)
		{
			
			if (_GUI_Window.code == WINDOW_ID::SELECT_MAP)
				game_setup_window();
			
			
			else if (_GUI_Window.code == WINDOW_ID::GAME_SETUP) //(inGameSetupScreen)
			{
				
				#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				if (netController.is_online() )
					netController.disconnect();
				#endif
				
				enter_main_menu();
				
			}
			
			else if (_GUI_Window.code == WINDOW_ID::SERVER_NOT_RESPONDING)
				online_window();
				
			else if (_GUI_Window.code == WINDOW_ID::MAIN_MENU)
			{
				#ifndef PLATFORM_WEB
				gameState = GAMESTATE::EXIT;
				#endif
			}
			else 
				enter_main_menu();
				
		}
		
		else 
			enter_main_menu();
		
	}
	
	else if (gameState == GAMESTATE::PLAYING)
	{
		
		if (editMode)
		{
			
			if (_GUI_Window.active)
				_GUI_Window.destroy();
			
			else
				esc_menu_window();
			
		}
		
		else
		{
			
			if (!_GUI_Window.active)
				esc_menu_window();
			
			else 
				leave_game();
			
		}
		
	}
	
}



void GameManager::GenerateMap() 
{
	
	gameMap.change_size(30, 30);
	
}



bool GameManager::play_area_sound(Sound & sound, float x, float y)
{
	
	float volume;
	
	const double distance = distance_between_points(camera.x, camera.y, x, y);
	//const float pan = cos( angle_between_points_radians(Camera.x, Camera.y, x1, y1) );
	
	// centraliza o som caso esteja nos limites da camera
	float pan = 0.0f;
	if (!point_visible_on_camera(x, y) )
		pan = cos( angle_between_points_radians(camera.x, camera.y, x, y) );
	
	const double MIN_FADE_DISTANCE = (double)(10 * TILE_SIZE);
	const double MAX_FADE_DISTANCE = (double)(26 * TILE_SIZE);
	
	if (distance >= MAX_FADE_DISTANCE)
		volume = 0.0f;
	
	else if (distance <= MIN_FADE_DISTANCE)
		volume = 1.0f;
	
	else
	{
		
		const double base_factor = distance - MIN_FADE_DISTANCE;
		const double max_factor = MAX_FADE_DISTANCE - MIN_FADE_DISTANCE;
		
		volume = 1.0f - (float)(base_factor / max_factor);
		
	}
	
	if (volume > 0.0f)
	{
		play_sound(sound, volume, pan);
		return true;
	}
	
	return false;
	
	//add_debug_message_to_console( to_stringi(x) + " " + to_stringi(y) + "| dist= " + to_stringi(distance) + "| volume= " + to_stringi(volume) );
	//add_debug_message_to_console("volume: " + to_stringi(volume) + " | pan: " + to_stringi(pan) );
	
}



bool GameManager::play_area_sound(Sound & sound, int x, int y)
{
	return play_area_sound(sound, (float)(x * TILE_SIZE + TILE_SIZE / 2), (float)(y * TILE_SIZE + TILE_SIZE / 2) );
}



bool GameManager::tile_completely_unoccupied(int x, int y)
{
    
    if (!tile_is_walkable(x, y, MOVEMENT_TYPE::NORMAL) )
        return false;
    
    if (tile_occupied(x, y, NULL) )
        return false;
    
    if (tile_occupied_by_object(x, y, NULL) )
        return false;
    
    return true;
    
}



bool GameManager::tile_occupied(int x, int y, const Ant * current)
{
	
	//int i = 0;
	for (Ant & entity : ant) 
	{
		if (entity.active && &entity != current)
		{
			
			IntVector2 tile_position = entity.get_tile_position();
			if (tile_position.x == x && tile_position.y == y)
				return true;
		}
		//i++;
	}
	return false;
	
}



bool GameManager::tile_occupied_by_object(int x, int y, const Ant * current)
{
	
	for (Ant & entity : ant) 
	{
		if (entity.active && (&entity != current) )
		{
			
			IntVector2 tile_position = entity.get_tile_position();
			if (tile_position.x == x && tile_position.y == y)
				return true;
		}
	}
	
	for (Powerup & entity : powerup) 
	{
		if (entity.active)
		{
			
			IntVector2 tile_position = entity.get_tile_position();
			if (tile_position.x == x && tile_position.y == y)
				return true;
		}
	}
	
	// objetos grandes, ocupando varios tiles
	for (Food & c_food : food) 
	{
		
		if (c_food.active)
		{
			const int width = c_food.get_tile_width();
			
			IntVector2 food_tile = c_food.get_tile_position();
			
			if (point_to_rectangle_collision(x, y, food_tile.x, food_tile.y, width, width) )
				return true;
		}
		
	}
	
	for (AntHill & c_antHill : antHill) 
	{
		if (c_antHill.active)
		{
			
			IntVector2 tilePosition = c_antHill.get_tile_position();
			if (x >= tilePosition.x - 1 && x <= tilePosition.x + 1 &&
				y >= tilePosition.y - 1 && y <= tilePosition.y + 1)
				return true;
			
		}
	}
	
	
	return false;
	
}




bool GameManager::tile_is_walkable(int x, int y, int8_t movementType)//, int8_t playerIndex)
{
	
	if (Tile * currentTile = gameMap.get_tile(x, y) )
		return tile_is_walkable(*currentTile, movementType);
	
	return false;
	
}



bool GameManager::tile_is_walkable(Tile & currentTile, int8_t movementType) //, int8_t playerIndex)
{
	
	if (currentTile.solid)
		return false;
	
	if (currentTile.type == TILE::WATER)
		return (currentTile.secondary == SECONDARY_TILE::MUD_BRIDGE || movementType == MOVEMENT_TYPE::SWIM);
	
	if (currentTile.secondary == SECONDARY_TILE::FIRE)
		return movementType == MOVEMENT_TYPE::FIRE;
	
	/*
	//if (currentTile.secondary >= SECONDARY_TILE::LANDMINE_0 && currentTile.secondary >= SECONDARY_TILE::LANDMINE_0
    if (currentTile.has_landmine() )
	{
		if (playerIndex < 0)
			return false;
		return currentTile.secondary != SECONDARY_TILE::LANDMINE_0 + playerIndex;
	}*/
	
	return true;
	
}



bool GameManager::tile_has_landmine(int x, int y, int8_t playerIndex)
{
	
	if (Tile * currentTile = gameMap.get_tile(x, y) )
	{
		if (currentTile->has_landmine() )
		{
			if (playerIndex < 0)
				return true;
			return currentTile->secondary == SECONDARY_TILE::LANDMINE_0 + playerIndex;
		}
	}
	
	return false;
	
}



void GameManager::calculate_solid_tiles()
{
	
	gameMap.reset_solid_marks();
	
	for (Obstacle & currentObstacle : obstacle)
	{
		
		const float width = currentObstacle.get_width();
		const float height = currentObstacle.get_height();
		//const float radius = width * 0.5f;
		const Vector2 & pos = currentObstacle.position;
		
		if (width <= TILE_SIZE && height <= TILE_SIZE)
			gameMap.mark_solid_tile(pos.x, pos.y);
		
		else
		{
			/*
			Vector2 topLeft(pos.x - radius, pos.y - radius); 
			IntVector2 intTopLeft( (int)(topLeft.x / TILE_SIZE), (int)(topLeft.y / TILE_SIZE) ); 
			
			Vector2 bottomRight(pos.x + radius, pos.y + radius); 
			IntVector2 intBottomRight( (int)(bottomRight.x / TILE_SIZE), (int)(bottomRight.y / TILE_SIZE) ); 
			*/
			
			Vector2 topLeft( currentObstacle.get_left(), currentObstacle.get_top() ); 
			IntVector2 intTopLeft( (int)(topLeft.x / TILE_SIZE), (int)(topLeft.y / TILE_SIZE) ); 
			
			Vector2 bottomRight(topLeft.x + width - 1, topLeft.y + height - 1); 
			IntVector2 intBottomRight( (int)(bottomRight.x / TILE_SIZE), (int)(bottomRight.y / TILE_SIZE) ); 
			
			
			gameMap.mark_solid_tiles(intTopLeft.x, intTopLeft.y, intBottomRight.x, intBottomRight.y);
			
		}
		
	}
	
	
	for (AntHill & currentAntHill : antHill)
	{
		if (currentAntHill.active && !currentAntHill.is_symmetrical() )
		{
			/*
			bool mask[] {1, 0, 1, 1,
						 1, 0, 1, 1,
						 1, 1, 1, 1,
						 1, 1, 1, 1}*/
			
			IntVector2 base_tile = currentAntHill.get_tile_position();
			
			for (int y = 0; y < 4; y++)
			{
				for (int x = 0; x < 4; x++)
				{
					if ( !(x == 1 && y == 0) && !(x == 1 && y == 1) )
						gameMap.mark_solid_tile(base_tile.x + x, base_tile.y + y);
				}
			}
			
		}
	}
	
}



void GameManager::place_console()
{
	
	//printf("place_console()\n ");
	
	float width = 500.0f;
	const float height = 256; //240.0f;
	
	const float offset = 20.0f;
	//const float bottom_offset = height + offset * 3.5f;
	
	float x, y;
	bool always_visible;
	bool place_on_window = (_GUI_Window.active && _GUI_Window.code == WINDOW_ID::GAME_SETUP);
	
	if (place_on_window)
	{
		x = _GUI_Window.x + offset;
		y = _GUI_Window.y + _GUI_Window.height - height - 20;
		width = _GUI_Window.width - offset * 2.0f;
		always_visible = true;
	}
	
	else // jogando
	{
		x = offset;
		y = screen_h - height - 20;
		always_visible = false;
	}
	
	console.set(x, y, width, height, always_visible, 1.0f, 32);
	console.full_hide = !place_on_window;
	//console.always_opaque = place_on_window;
	
	//console.debug_printf();
	
}



void GameManager::add_message_to_the_console(const std::string & message, Player * sender)
{
	
	std::string player_name = sender ? sender->name : "";
	ColorRGBA player_color = sender ? sender->get_color() : WHITE;
	
	//consoleMessage.push_back( ChatString(message, player_name, player_color) );
	console.add_message( ChatString(message, player_name, player_color) );
	
	
	
}


void GameManager::add_chat_message_to_the_console(const std::string & message, Player * sender)
{
	add_message_to_the_console(message, sender);
	play_sound(s_chat);
}



Vector2 GameManager::get_mouse_world_position()
{
    
    return camera.mouse_coordinates_to_world_coordinates();
	
	/*
	const float screen_width = screen_w;
	const float screen_height = screen_h;
	Vector2 raw_mouse_position = GetMousePosition();
	float inv_zoom = 1.0 / camera.zoom;
	
	Vector2 world_position;
	world_position.x = camera.target.x + (raw_mouse_position.x - screen_width * 0.5f) * inv_zoom;
	world_position.y = camera.target.y + (raw_mouse_position.y - screen_height * 0.5f) * inv_zoom;
	
	return world_position;
	*/
}



FoodData * GameManager::get_food_data(int food_id)
{
	
	for (FoodData & current : foodData)
	{
		if (current.id == food_id)
			return &current;
	}
	return NULL; 
	
}



ObstacleData * GameManager::get_obstacle_data(int obstacle_id)
{
	
	for (ObstacleData & current : obstacleData)
	{
		if (current.id == obstacle_id)
			return &current;
	}
	return NULL; 
	
}



Bitmap * GameManager::get_food_sprite(int food_id)
{
	
	for (FoodData & current : foodData)
	{
		if (current.id == food_id)
			return &current.sprite;
	}
	return &missingSprite; 
	
}



String GameManager::get_powerup_name(int id)
{
	
	switch(id)
	{
		
		case POWERUP::NONE: return "None";
		case POWERUP::STRENGTH: return "Strength";
		case POWERUP::FIREMAN: return "Fireman";
		case POWERUP::ROBBER: return "Robber";
		case POWERUP::DIVER: return "Diver";
		case POWERUP::BOMBER: return "Bomber";
		
	}
	return "?";
	
}



bool GameManager::GameObject_inside_rectangle(float x, float y, float width, float height, GameObject & gameObject)
{
	
	return (rectangle_collision(x, y, width, height,
						gameObject.position.x - TILE_SIZE / 2, gameObject.position.y - TILE_SIZE / 2, (float)TILE_SIZE, (float)TILE_SIZE) );
	
}


bool GameManager::point_on_GameObject(float x, float y, GameObject & gameObject)
{
	
	return (point_to_rectangle_collision_topleft(x, y, 
						gameObject.position.x - TILE_SIZE / 2, gameObject.position.y - TILE_SIZE / 2, (float)TILE_SIZE, (float)TILE_SIZE) );
						
}


bool GameManager::points_close(float x1, float y1, float x2, float y2, float tolerance)
{
	return (distance_between_points_squared(x1, y1, x2, y2) <= tolerance * tolerance);
}



bool GameManager::point_visible_on_camera(float x, float y)
{
	//return CheckCollisionPointRec( (Vector2){ x - camera.target.x, y - camera.target.y }, (Rectangle){ 0, 0, screen_w / camera.zoom, screen_h / camera.zoom } );
	return camera.object_visible(x, y);
}


/*
ColorRGBA GameManager::get_default_indexed_color(int index)
{
	
	if (index == 0) return color_int_to_float(88, 196, 103, 255); //ColorRGBA {0, 255, 0, 255};
	if (index == 1) return color_int_to_float(236, 80, 103, 255); //ColorRGBA {255, 0, 0, 255};	
	if (index == 2) return color_int_to_float(101, 129, 245, 255); //ColorRGBA {0, 0, 255, 255};	
	if (index == 3) return color_int_to_float(170, 163, 186, 255); //ColorRGBA {128, 128, 140, 255};	
	return WHITE;
	
}*/



ColorRGBA GameManager::get_health_tint(float factor)
{
	
	if (factor >= 0.5f)
		return ColorRGBA( (1.0f - factor) * 2.0f, 1.0f, 0.0f, 1.0f);
	
	return ColorRGBA(1.0f, factor * 2.0f, 0.0f, 1.0f);
	
}



bool GameManager::mouse_over_UI()
{
	
	if (_GUI_Window.active && _GUI_Window.mouse_over() )
		return true;
		
	if (mapEditor_upperMenu.active && mapEditor_upperMenu.mouse_over() )
		return true;
		
	if (mapEditor_toolWindow.active && mapEditor_toolWindow.mouse_over() )
		return true;
		
	if (console.active && console.currently_visible() && console.mouse_inside() )
		return true;
		
	return false;
	
}



bool GameManager::play_random_ingame_music()
{
	
	if (musicDisabled) return false;
	
	bool success = false;
	const String basePath = "Data/Musics/";
	
	FileEntry fe;
	if (fe.open_directory(basePath.c_str() ) )
	{
		std::vector<String> filenames;
		while(fe.read_directory() )
		{
			
			String filename = fe.get_entry_name();
			if (filename != "INTRO.ogg" && filename != currentMusicFileName)
				filenames.push_back(filename);
			
		}
		
		fe.close_directory();
		
		
		if (!filenames.empty() )
		{
			
			size_t index = iRand(0, filenames.size() - 1);
			String path = basePath + filenames[index];
			success = play_music(path, false);
			
			currentMusicFileName = success ? filenames[index] : "";
			
		}
		
	}
	
	return success;
	
}


bool GameManager::remove_object_at_tile(int x, int y)
{
	
	size_t i = 0;
	for (const AntHill & current : antHill) 
	{
		
		IntVector2 tilePosition = current.get_tile_position();
		if (tilePosition.x == x && tilePosition.y == y)
		{
			antHill.erase( antHill.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const Ant & current : ant) 
	{
		
		IntVector2 tilePosition = current.get_tile_position();
		if (tilePosition.x == x && tilePosition.y == y)
		{
			ant.erase( ant.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const Food & current : food) 
	{
		
		if (current.position.x == x && current.position.y == y)
		{
			food.erase( food.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const Powerup & current : powerup) 
	{
		
		IntVector2 tilePosition = current.get_tile_position();
		if (tilePosition.x == x && tilePosition.y == y)
		{
			powerup.erase( powerup.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const Obstacle & current : obstacle) 
	{
		
		if (current.position.x == x && current.position.y == y)
		{
			obstacle.erase( obstacle.begin() + i);
			calculate_solid_tiles();
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const PowerupGenerator & current : powerupGenerator) 
	{
		
		if (current.position.x == x && current.position.y == y)
		{
			powerupGenerator.erase( powerupGenerator.begin() + i);
			return true;
		}
		i++;
		
	}
	
	return false;
	
}



void GameManager::center_camera()
{
	camera.move_to(gameMap.width * TILE_SIZE / 2, gameMap.height * TILE_SIZE / 2);
}
