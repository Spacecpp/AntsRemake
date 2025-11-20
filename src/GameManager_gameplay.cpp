#include "GameManager.h"
#include "General_func.h"
#include "net_commands_list.h"
#include "Colors.h"



bool GameManager::game_can_start()
{
	
	// player count > 0
	
	if (!gameMap.loaded)
	{
		printf("Map not loaded.\n");
		return false;	
	}
	
	// checar se sem start pra todos
	
	// checar se todos tem o mapa, e o checksum
	
	for (Player & current : player)
	{
		if (current.active && !current.ready)
		{
			printf("Not all players are ready.\n");
			return false;	
		}
	}
	
	// checa repeticao de cor
	size_t i = 1;
	for (std::vector<Player>::iterator it = player.begin(); it != player.end(); ++it)
	{
		
		Player & player1 = *it;
		if (player1.active)
		{
			
			for (std::vector<Player>::iterator it2 = player.begin() + i; it2 != player.end(); ++it2)
			{
				
				Player & player2 = *it2;
				if (player2.active)
				{
					
					if (&player1 != &player2 && player1.colorIndex == player2.colorIndex)
					{
						printf("2 or more players have the same color\n");
						return false;
					}
					
				}
				
			}
			
		}
		
		i += 1;
		
	}

	return true;
	
}


void GameManager::attempt_to_start_game()
{
	
	if (game_can_start() )
		start_game();
}



void GameManager::setup_new_game()
{
	
	player.clear();
	player.reserve(MAX_PLAYERS);
	playerNextUniqueID = 1;
	
	//player.push_back( Player(localPlayerName, get_indexed_color(0), CONTROLLER::LOCAL, playerNextUniqueID++, SERVER_CONNECTION_ID) );
	player.push_back( Player(localPlayerName, 0, CONTROLLER::LOCAL, playerNextUniqueID++, SERVER_CONNECTION_ID) );
	
	find_local_player();
	
}


/*
void GameManager::start_single_player_game()
{
	
	player.clear();
	player.reserve(MAX_PLAYERS);
	playerNextUniqueID = 1;
	
	// preload map para determinar os players
	std::string mappath = "Maps/" + selectedMapName + ".map";
	load_map(mappath.c_str() );
	
	int anthillCount = (int)gameMap.antHillSpawn.size();
	printf("anthillCount: %d\n", anthillCount);
	
	for (int index = 0; index < anthillCount; index++) 
	{
		
		if (index == 0)
			add_player( Player("Player", 0, CONTROLLER::LOCAL, playerNextUniqueID++, 0) );
		else
			add_player( Player("Bot " + to_stringi(index + 1), index, CONTROLLER::AI, playerNextUniqueID++, 0) );
		
	}
	
	//add_player( Player("Player", get_indexed_color(0), CONTROLLER::LOCAL, playerNextUniqueID++, 0) );
	//add_player( Player("Bot 1", get_indexed_color(1), CONTROLLER::AI, playerNextUniqueID++, 0) );
	//add_player( Player("Bot 2", get_indexed_color(2), CONTROLLER::AI, playerNextUniqueID++, 0) );
	//add_player( Player("Bot 3", get_indexed_color(3), CONTROLLER::AI, playerNextUniqueID++, 0) );
	
	
	//player.push_back( Player("Player", get_indexed_color(0), CONTROLLER::LOCAL, playerNextUniqueID++) );
	//player.push_back( Player("Dummy", get_indexed_color(1), CONTROLLER::AI, playerNextUniqueID++) );
	
	if (!player.empty() )
		localPlayer = &player[0];
	else
		localPlayer = NULL;
	
	start_game();
	
}*/


void GameManager::setup_single_player_game()
{
	
	player.clear();
	player.reserve(MAX_PLAYERS);
	playerNextUniqueID = 1;
	
	// preload map para determinar os players
	load_selected_map();
	
	
	int anthillCount = (int)gameMap.antHillSpawn.size();
	printf("anthillCount: %d\n", anthillCount);
	
	for (int index = 0; index < anthillCount; index++) 
	{
		
		if (index == 0)
			add_player( Player("Player", 0, CONTROLLER::LOCAL, playerNextUniqueID++, 0) );
		else
			add_player( Player("Bot " + to_stringi(index), index, CONTROLLER::AI, playerNextUniqueID++, 0) );
		
	}
	
	for (Player & current : player)
		current.ready = true;
		
	
	if (!player.empty() )
		localPlayer = &player[0];
	else
		localPlayer = NULL;
	
	game_setup_window();
	
}



void GameManager::enter_map_editor()
{
    
    //start_single_player_game();
    editMode = true;
    editorSaveFilename.clear();
    stop_music();
    
    gameState = GAMESTATE::PLAYING;
	inGameSetupScreen = false;
	gameInProgress = true;
	
	_GUI_Window.destroy();
	
	reset_gameplay();
	
	editorTileType = 0;
	editorElementType = 0;
	editorVariant = 1;
	editorPowerupGenTime = 60;
	editorAntHillFlags = 0;
	editorPlayerID = 0;
	editorPage = 0;
	
	console.clear_messages();
	console.destroy();
    
	draggingBoxSelection = false;
	
	
	// cria dummy players
	player.clear();
	player.reserve(MAX_PLAYERS);
	playerNextUniqueID = 1;
	
	for (int index = 0; index < MAX_PLAYERS; index++) 
	{
		add_player( Player("Player " + to_stringi(index + 1), index, CONTROLLER::LOCAL, playerNextUniqueID++, 0) );
	}
	
	// mapa basico
	gameMap.reset();
	gameMap.change_size(20, 20);
	gameMap.gameTime = 10 * 60;
	
	refresh_camera();
    camera.zoom = 2.0f;
    camera.move_to(gameMap.width * TILE_SIZE / 2, gameMap.height * TILE_SIZE / 2);
    
    //info_window("The map editor is in a very early state. A proper interface will come in future builds");
    
    create_map_editor_upper_menu_window();
    create_map_editor_tool_window();
    
    
    // debug
    for (Player & current : player)
		current.debug_printf();
    
}


/*
void GameManager::start_multiplayer_game()
{
	
	if (!is_online() )
	{
		printf("You are not online\n");
		return;
	}
	
	printf("start_multiplayer_game()\n");
	
	find_local_player();
	
	start_game();
	
}*/


void GameManager::setup_multiplayer_game()
{
	
	player.clear();
	player.reserve(MAX_PLAYERS);
	playerNextUniqueID = 1;
	
	inGameSetupScreen = true;
	
	//player.push_back( Player("Player 1", get_indexed_color(0), CONTROLLER::LOCAL, playerNextUniqueID++) );
	//add_player( Player("Player 1", get_indexed_color(0), CONTROLLER::LOCAL, playerNextUniqueID++, SERVER_CONNECTION_ID) );
	add_player( Player("Player 1", 0, CONTROLLER::LOCAL, playerNextUniqueID++, SERVER_CONNECTION_ID) );
	
	find_local_player();
	
}



void GameManager::start_game()
{
	
	#ifdef ONLINE_MULTIPLAYER_AVAILABLE
	if (netController.server_running() )
	{
		
		uint32_t random_seed = gameRNG.random_number();
		gameRNG.set_seed(random_seed);
		//printf("Seed generated: %u\n", random_seed);
		
		Server_comunicate_game_start();
	}
	#endif
	
	find_local_player();
	
	gameState = GAMESTATE::PLAYING;
	inGameSetupScreen = false;
	gameInProgress = true;
	editMode = false;
	
	_GUI_Window.destroy();
	
	reset_gameplay();
	
	place_console();
	console.clear_messages();
	//console.always_opaque = false;
	
	refresh_camera();
	
    camera.zoom = 2.0f;
	
	draggingBoxSelection = false;
	
	
	#ifdef ONLINE_MULTIPLAYER_AVAILABLE
	if (netController.server_running() )
        networkFrame = commandDelay / 2; // servidor espera um pouco pelos updates dos clientes
	#endif
	
	//if (load_map("Maps/map1.map") )
	
	if (!gameMap.loaded)
	{
		std::string mappath = "Maps/" + selectedMapName + ".map";
		load_map(mappath.c_str() );
	}
	
	if (gameMap.loaded)
	{
		
		gameTime = gameMap.gameTime * 60;
		if (gameTime <= 0)
            gameTime = 60 * 60 * 10; // 10 minutos
		
		// cria objetos
		convert_map_objects_into_gameplay_format();
		/*
		for (const IntVector3 & current : gameMap.antHillSpawn) 
		{
			if (Player * current_player = get_Player_by_index(current.z) )
				antHill.push_back( AntHill(current.x, current.y, current_player) );
			else
				printf("Player %d not found\n", current.z);
		}
		
		for (const IntVector3 & current : gameMap.antSpawn) 
		{
			if (Player * current_player = get_Player_by_index(current.z) )
				create_Ant( Ant(current.x, current.y, current_player, antNextUniqueID++) );
			else
				printf("Player %d not found\n", current.z);
		}
		
		for (const IntVector3 & current : gameMap.foodSpawn) 
		{
			food.push_back( Food(current.x, current.y, current.z) );
		}
		
		for (const IntVector3 & current : gameMap.itemSpawn) 
		{
			powerup.push_back( Powerup(current.x, current.y, current.z) );
		}
		
		for (const IntVector3 & current : gameMap.obstacleSpawn) 
		{
			obstacle.push_back( Obstacle(current.x, current.y, current.z) );
		}*/
		
		printf("Ants %d, Food %d, Powerups %d\n", (int)ant.size(), (int)food.size(), (int)powerup.size() );
		
	}
	
	else
	{
		
		printf("Map not loaded. Generating one\n");
		
		GenerateMap();
	
		gameTime = 60 * 60 * 10;
		
		if (player.size() > 0)
		{
			
			antHill.push_back( AntHill(8, 6, &player[0], 0) );
			
			create_Ant( Ant(0, 0, &player[0], antNextUniqueID++) );
			create_Ant( Ant(9, 9, &player[0], antNextUniqueID++) );
			create_Ant( Ant(8, 1, &player[0], antNextUniqueID++) );
			create_Ant( Ant(2, 9, &player[0], antNextUniqueID++) );
			
		}
		
		food.push_back( Food(14, 5, 0) );
		
		powerup.push_back( Powerup(15, 10, 0) );
		
	}
	
	calculate_solid_tiles();
	
	// centraliza a camera no jogador
	if (localPlayer)
	{
		if (AntHill * myHill = get_AntHill(localPlayer) )
            camera.move_to(myHill->position.x, myHill->position.y);
		else
			printf("Can't find local player anthill\n");
			//camera.target = (Vector2) { myHill->position.x, myHill->position.y };
	}
	
	
	for (Player & currentPlayer : player)
		printf("%s\n", currentPlayer.name.c_str() );
	
	
	printf("Local player: %s\n", localPlayer ? localPlayer->name.c_str() : "Local player is NULL" );
	
	stop_music();
	
	play_random_ingame_music();
	
}


void GameManager::update_gameframe()
{
    
    const bool is_online_ = is_online();
    const bool is_client_ = is_online_ && is_client();
    const bool is_server_ = is_online_ && !is_client_;
    
    if (gameFrame >= gameFrameMax)
    {
        
        bool can_advance = false;
        if (is_online_)
        {
            
            if (is_server_)
            {
                
                bool problem = false;
                
                // precisa que todos os clientes estejam sincronizados
                for (Player & currentPlayer : player)
                {
                    if (currentPlayer.active && currentPlayer.controller == CONTROLLER::REMOTE && !currentPlayer.disconnected() )
                    {
                        //if (currentPlayer.gameFrame < gameFrame)
                        if (currentPlayer.gameFrame < gameFrameMax)
                        {
                            problem = true;
                            break;
                        }
                    }
                }
                
                can_advance = !problem;
                
            }
            
        }
        else can_advance = true;
        
        if (can_advance)
            gameFrameMax += std::max( (uint32_t)1, commandDelay);
    
    }
    
    if (gameFrame < gameFrameMax)
    {
        gameFrame++;
        gameFrameDone = false;
    }
    
    
    #ifdef ONLINE_MULTIPLAYER_AVAILABLE
    if (is_online_)
    {
        
        if (pendingResync && is_server_)
        {
            
            pendingResync = false;
            
            BitStream data;
            data.addInt(OP::RESYNC, bytesOf(OP::COUNT) );
            pack_resync(data);
            netController.send_packet_to_all_clients(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
            
            add_message_to_the_console("Resync packet sent");
            
        }
        
        // se alcancou o networkFrame, envia as ordens buferizadas
        else if (gameFrame == networkFrame /*gameState == GAMESTATE::PLAYING && !gameEnded &&*/ )
        {
            
            // kludge: todas as ordens datadas de gf antigo vao pro atual. deve se tornar desnecessario quando o algoritimo estiver ok
            for (GameOrder & currentOrder : netQueuedOrder)
            {
                if (currentOrder.targetGameFrame < gameFrame)
                {
                    int delta = gameFrame - currentOrder.targetGameFrame;
                    printf("o %d to %d (delta %d)\n", currentOrder.targetGameFrame, gameFrame, delta);
                    
                    if (is_server_)
                        currentOrder.targetGameFrame = gameFrame;
                        
                    else if (is_client_)
                    {
                        // pedir resync?
                        add_message_to_the_console("Outdated packet received");
                    }
                }
            }
            
            // comunica
            if (is_client_ || netController.get_number_of_users_connected() > 0)
                communicate_update();
                
            
            // servidor passa todas as ordens para a fila de jogo
            if (is_server_)
            {
                queuedOrder.insert( queuedOrder.end(), netQueuedOrder.begin(), netQueuedOrder.end() );
            }
            
            netQueuedOrder.clear();
            
            // seta o networkFrame futuro
            networkFrame += commandDelay;
            
            //if (is_server_) 
            //	gameFrameMax = networkFrame;
            
            Net_process_output();
            
        }
    
    }
    #endif
    
}


void GameManager::update_gameplay()
{

	// gameplay de fato
	if (!gameEnded && !editMode && gameFrame <= gameFrameMax && !gameFrameDone)
	{
		
		if (!is_client() )
			update_AI_players();
		
		update_order_queue();
		
		for (AntHill & c_antHill : antHill) 
		{
			if (c_antHill.active)
				c_antHill.update();
		}
		
		for (Ant & c_ant : ant) 
		{
			if (c_ant.active)
				c_ant.update();
		}
		
		for (Food & c_food : food) 
		{
			if (c_food.active)
				c_food.update();
		}
		
		for (FlyingText & c_flyingText : flyingText) 
		{
			if (c_flyingText.active)
				c_flyingText.update();
		}
		
		for (HealParticle & c_healParticle : healParticle) 
		{
			if (c_healParticle.active)
				c_healParticle.update();
		}
		
		for (AnimatedSprite & c_animatedSprite : animatedSprite) 
		{
			if (c_animatedSprite.active)
				c_animatedSprite.update();
		}
		
		for (PowerupGenerator & c_powerupGenerator : powerupGenerator) 
		{
			if (c_powerupGenerator.active)
				c_powerupGenerator.update();
		}
		
		
		
		resolve_collisions();
		
		// lutas
		for (Fight & c_Fight : fight) 
		{
			//if (c_Fight.active)
				c_Fight.update();
		}
		fight.clear();
		
		// evita bug de anthill permanentemente travado
		for (AntHill & c_antHill : antHill) 
		{
			if (c_antHill.active)
				c_antHill.check_auto_unlock();
		}
		
		
		// passagem do tempo
		if (gameTime > 0)
		{
			
			gameTime--;
			
			// alarme no final do jogo, 10 ultimos segundos
			if (gameTime > 0 && gameTime <= 10 * 60 && gameTime % 60 == 0)
			{
				play_sound(s_alarm);
			}
			
			if (gameTime == 0)
			{
				
				gameEnded = true;
				
				if (localPlayer)
				{
					
					bool victory = false;
					
					// TODO: checar por empate
					std::vector<Player*> sortedPlayers;
					for (Player & currentPlayer : player)
					{
						if (currentPlayer.active)
							sortedPlayers.push_back(&currentPlayer);
					}
					std::sort(sortedPlayers.begin(), sortedPlayers.end(), sortPlayerScore);
					
					if (localPlayer == sortedPlayers.front() )
						victory = true;
						
					play_sound(victory ? s_victory : s_defeat);
					
				}
				
				end_game_window();
				
			}
			
		}
		
		//gameFrame++;
		gameFrameDone = true;
		
	}
	
	
	// somente animacao
	else if (editMode)
	{
		
		for (Food & c_food : food) 
		{
			if (c_food.active)
				c_food.update();
		}
		
	}
	
	// ----- animacao ----
	waterAnimationOffset += 0.05f;
	if (waterAnimationOffset >= 32.0f)
		waterAnimationOffset -= 32.0f;
	
	// fogo
	fireAnimationDelay--;
	if (fireAnimationDelay <= 0)
	{
		fireAnimationDelay = global_fireAnimationDelay;
		fireAnimationFrame++;
		
		if (fireAnimationFrame >= global_fireAnimationFrames)
			fireAnimationFrame = 0;
	}
	
	// minas
	mineAnimationDelay--;
	if (mineAnimationDelay <= 0)
	{
		mineAnimationDelay = 5;
		mineAnimationFrame++;
		
		if (mineAnimationFrame >= 2)
			mineAnimationFrame = 0;
	}
	
	// animacao das bandeiras
	flagAnimationTime--;
	if (flagAnimationTime <= 0)
	{
		
		flagAnimationTime = 10;
		
		flagAnimationFrame++;
		if (flagAnimationFrame >= FLAG_TOTAL_ANIMATION_FRAMES)
			flagAnimationFrame = 0;
		
	}

}



Player * GameManager::add_player(const Player & newPlayer)
{
	
	size_t index = 0;
	for (Player & current : player)
	{
		if (!current.active)
		{
			
			current = newPlayer;
			current.index = index;
			create_player_setup_widgets(current);
			return &current;
			
		}
		index++;
	}
	
	if (player.size() < MAX_PLAYERS)
	{
		
		player.push_back(newPlayer);
		Player & current = player.back();
		current.index = index;
		create_player_setup_widgets(current);
		return &player.back();
		
	}
	
	printf("Failed to add new player\n");
	return NULL;
	
}



bool GameManager::remove_player(Player & targetPlayer)
{
	
	for (Player & current : player)
	{
		if (current.active && (&current == &targetPlayer) )
		{
			
			remove_player_setup_widgets(targetPlayer);
			current.destroy();
			return true;
			
		}
	}
	
	printf("Could't find player to remove\n");
	return false;
	
}



bool GameManager::add_bot()
{

	//add_player( Player("Player", get_indexed_color(0), CONTROLLER::AI, playerNextUniqueID++, 0) );
	
	size_t index = 0;
	for (Player & current : player)
	{
		if (!current.active)
		{
			
			current = Player("Bot " + to_stringi(index), index, CONTROLLER::AI, playerNextUniqueID++, 0);
			current.index = index;
			current.ready = true;
			create_player_setup_widgets(current);
			return true;
			
		}
		index++;
	}
	
	if (player.size() < MAX_PLAYERS)
	{
		
		player.push_back( Player("Bot " + to_stringi(index), index, CONTROLLER::AI, playerNextUniqueID++, 0) );
		Player & current = player.back();
		current.index = index;
		create_player_setup_widgets(current);
		return true;
	}
	
	printf("Failed to add bot\n");
	return false;
	
}


bool GameManager::remove_bot()
{
	
	for (std::vector<Player>::reverse_iterator it = player.rbegin(); it != player.rend(); ++it)
	{
		Player & currentPlayer = *it;
		if (currentPlayer.active && currentPlayer.controller == CONTROLLER::AI)
		{
			
			remove_player_setup_widgets(currentPlayer);
			currentPlayer.destroy();
			return true;
			
		}
	}
	
	printf("Could't find bot to remove\n");
	return false;
	
}


void GameManager::resolve_collisions()
{
	
	const float map_max_x = gameMap.width * TILE_SIZE;
	const float map_max_y = gameMap.height * TILE_SIZE;
	
	const float ant_radius = ANT_RADIUS;
	const float ant_collision_distance_squared = (ant_radius * 2.0f) * (ant_radius * 2.0f);
	//const float repulsion_factor = 2.0f;
	//const float slide_factor = 2.0f;
	
	// verifica colisoes entre as formigas
	size_t i = 0;
	for (Ant & ant1 : ant) 
	{
		if (ant1.active && ant1.elegible_for_collisions() )
		{
			
			// checa com todas as outras formigas
			//for (Ant & ant2 : ant) 
			for (std::deque<Ant>::iterator it = ant.begin() + (i + 1); it < ant.end(); ++it)
			{
				
				Ant & ant2 = *it;
				if (ant2.active && (&ant2 != &ant1) && ant2.elegible_for_collisions() && ant1.can_collide_with(ant2) )
				{
					
					const float distance_squared = distance_between_points_squared(ant1.position.x, ant1.position.y, ant2.position.x, ant2.position.y);
					
					// colisao
					if (distance_squared < ant_collision_distance_squared)
					{
						
						//const float interference_distance = sqrt(ant_collision_distance_squared - distance_squared); // NAO eh a mesma coisa
						const float interference_distance = sqrt(ant_collision_distance_squared) - sqrt(distance_squared);
						
						//const float distance = sqrt(distance_squared);
						
						const float angle_to_other_rad = ant1.angle_to_rad(ant2);
						const float angle_to_other_deg = degrees_to_radians(angle_to_other_rad);
						
						float angle_to_move = angle_to_other_rad;
						
						if ( fabs(difference_between_angles(ant1.angle, angle_to_other_deg) ) < 5.0f &&
							 fabs(difference_between_angles(ant1.angle, ant2.angle + 180.0f) ) < 5.0f)
						{
							angle_to_move += degrees_to_radians(15.0f); 
							//printf("colisao jogando pro lado\n");
						}
						
						if (!ant1.pushBlocked)
							ant1.move_rad(interference_distance * -0.5f, angle_to_move);
						
						if (!ant2.pushBlocked)
							ant2.move_rad(interference_distance * 0.5f, angle_to_move);
						
						
						if (!ant1.pushBlocked && !ant2.pushBlocked && 
							ant1.both_wants_to_go_to_the_same_position(ant2) )
						{
							ant1.clear_path();
							ant2.clear_path();
						}
						
						/*
						const float dx = (ant1.position.x - ant2.position.x) / distance * repulsion_factor;
						const float dy = (ant1.position.y - ant2.position.y) / distance * repulsion_factor;
						
						bool stopped1 = ant1.actionDelay > 0 || ant1.reached_destination();
						bool stopped2 = ant2.actionDelay > 0 || ant2.reached_destination();
						
						float angle_difference = difference_between_angles(ant1.angle, ant2.angle);
						bool should_slide = angle_difference > 120.0f;
						
						
						// float angle_difference = fabs(ant1.angle - ant2.angle);
						// if (angle_difference > 180.0f)
							// angle_difference -= 180.0f;
						
						
						if (!stopped1)
						{
							
							ant1.position.x += dx;
							ant1.position.y += dy;
							
							if (should_slide || stopped2)
							{
								const float rad_angle_p = degrees_to_radians(ant1.angle + 45.0f);
								ant1.position.x += cos(rad_angle_p) * slide_factor;
								ant1.position.y += sin(rad_angle_p) * slide_factor;
							}
							
						}
						
						if (!stopped2)
						{
							
							ant2.position.x -= dx;
							ant2.position.y -= dy;
							
							if (should_slide || stopped1)
							{
								const float rad_angle_p = degrees_to_radians(ant2.angle + 45.0f);
								ant2.position.x += cos(rad_angle_p) * slide_factor;
								ant2.position.y += sin(rad_angle_p) * slide_factor;
							}
							
						}*/
						
					}
					
					
					
				}
				
			}
			
		}
		
		i++;
		
	}
	
	
	// verifica colisoes com o terreno
	for (Ant & ant1 : ant) 
	{
		
		if (ant1.active)
		{
			
			//bool out_of_bounds = false;
			
			if (ant1.elegible_for_collisions() )
			{
				
				IntVector2 myTilePosition = ant1.get_tile_position();
				Tile * currentTile = gameMap.get_tile(myTilePosition.x, myTilePosition.y);
				if (currentTile)
				{
					
					const int8_t movement_type = ant1.get_movement_type();
					
					// empurra pra fora do tile
					if (ant1.HP > 0 && !tile_is_walkable(*currentTile, movement_type) )
					{
						
						/*
						Vector2 tileCenter { myTilePosition.x * TILE_SIZE + TILE_SIZE * 0.5f, myTilePosition.y * TILE_SIZE + TILE_SIZE * 0.5f };
						
						
						//const float distance_to_center2 = distance_between_points_squared(ant1.position.x, ant1.position.y, tileCenter.x, tileCenter.y);
						const float distance_to_center = distance_between_points(ant1.position.x, ant1.position.y, tileCenter.x, tileCenter.y);
						
						float rad_angle;
						float distance_to_move;
						
						rad_angle = ant1.angle_to_rad(tileCenter.x, tileCenter.y);
						
						//float dx = distance_to_center * cos(rad_angle);
						//float dy = distance_to_center * sin(rad_angle);
						float dx = (TILE_SIZE * 0.5f) * cos(rad_angle);
						float dy = (TILE_SIZE * 0.5f) * sin(rad_angle);
						
						ant1.position.x = tileCenter.x - dx;
						ant1.position.y = tileCenter.y - dy;
						*/
						
						//ant1.position.x = tileCenter.x - distance_to_center * cos(rad_angle);
						//ant1.position.y = tileCenter.y - distance_to_center * sin(rad_angle);
						
						/*
						// empurra com base de onde veio
						if (distance_to_center2 <= 2.0f)
						{
							
							rad_angle = degrees_to_radians(ant1.angle);
							distance_to_move = TILE_SIZE * 0.5f;
							
						}
						
						// empurra em relacao ao angulo
						else 
						{
							
							rad_angle = ant1.angle_to_rad(tileCenter.x, tileCenter.y);
							distance_to_move = -sqrt(distance_to_center2);
							
						}
						
						//ant1.move_rad(distance_to_move, rad_angle);
						
						float dx = ant1.position.x - tileCenter.x;
						float dy = ant1.position.y - tileCenter.y;
						
						*/
						
						// se foi atirado encima do tile, continua pulando
						if (ant1.oldPosition_z > 0.0f)
							ant1.add_force(ant1.angle, -3.0f, GRAVITY_FORCE * 6.0f);
						
						// volta a posicao
						else
						{
						
							bool collision_x = !tile_is_walkable( (int)(ant1.position.x / TILE_SIZE), (int)(ant1.oldPosition.y / TILE_SIZE), movement_type);
							bool collision_y = !tile_is_walkable( (int)(ant1.oldPosition.x / TILE_SIZE), (int)(ant1.position.y / TILE_SIZE), movement_type);
							bool both = (collision_x == collision_y);
							
							
							if (collision_x || both)
								ant1.position.x = ant1.oldPosition.x;
							
							if (collision_y || both)
								ant1.position.y = ant1.oldPosition.y;
						
						}
						
					}
					
				}
				//else out_of_bounds = true;
				
			}
			
			// prende nos limites do mapa
			/*
			if (out_of_bounds)
			{
				
				if (ant1.position.x < 0.0f)
					ant1.position.x = 0.0f;
				
				else if (ant1.position.x > map_max_x)
					ant1.position.x = map_max_x;
				
				if (ant1.position.y < 0.0f)
					ant1.position.y = 0.0f;
				
				else if (ant1.position.y > map_max_y)
					ant1.position.y = map_max_y;
					
			}*/
			
			if (ant1.position.x - ANT_RADIUS < 0.0f)
				ant1.position.x = ANT_RADIUS;
			
			else if (ant1.position.x + ANT_RADIUS > map_max_x)
				ant1.position.x = map_max_x - ANT_RADIUS;
			
			if (ant1.position.y - ANT_RADIUS< 0.0f)
				ant1.position.y = ANT_RADIUS;
			
			else if (ant1.position.y + ANT_RADIUS > map_max_y)
				ant1.position.y = map_max_y - ANT_RADIUS;
			
			
			
		}
		
	}
	
}


bool GameManager::create_powerup_around_tile(int x, int y, int powerupID, bool allow_center)
{
    
    if (gameMap.coordinates_out_of_bounds(x, y) )
        return false;
    
    if (allow_center && tile_completely_unoccupied(x, y) )
    {
        create_Powerup( Powerup(x, y, powerupID) );
        return true;
    }
    
    // funcao sem otimizacao!
    for (int distance = 1; distance < 5; distance++)
    {
        
        for (int y1 = y - distance; y1 <= y + distance; y1++)
        {
            for (int x1 = x - distance; x1 <= x + distance; x1++)
            {
                
                if (tile_distance(x, y, x1, y1) == distance)
                {
                    
                    if (tile_completely_unoccupied(x1, y1) )
                    {
                        create_Powerup( Powerup(x1, y1, powerupID) );
                        return true;
                    }
                    
                }
                
            }
        }
        
    }
    
    // fallback
    create_Powerup( Powerup(x, y, powerupID) );
    return true;
    
}


size_t GameManager::get_player_count()
{
    
    size_t count = 0;
    for (Player & current : player) 
    {
        if (current.active)
            count++;
    }
    return count;
    
}


size_t GameManager::get_ant_count()
{
    
    size_t count = 0;
    for (Ant & current : ant) 
    {
        if (current.active)
            count++;
    }
    return count;
    
}


size_t GameManager::get_powerup_count()
{
    
    size_t count = 0;
    for (Powerup & current : powerup) 
    {
        if (current.active)
            count++;
    }
    return count;
    
}


size_t GameManager::get_next_player_free_index()
{
    
    size_t index = 0;
    for (Player & current : player) 
    {
        
        if (!current.active)
            break;
        
        index++;
        
    }
    return index;
    
}


void GameManager::set_selected_ants_control_group(int id)
{
	
	if (!selectedAnt.empty() )
	{
		
		// limpa de outras formigas que ja estiverem nesse grupo
		for (Ant & currentAnt : ant)
		{
			if (currentAnt.active && currentAnt.groupID == id)
				currentAnt.groupID = 0;
		}
		
		// seta nas formigas selecionadas
		for (Ant * currentAnt : selectedAnt)
		{
			if (currentAnt && currentAnt->active)
				currentAnt->groupID = id;
		}
		
	}
	
}


void GameManager::select_control_group(int id)
{
	
	std::vector<Ant*> antsFound;
	
	for (Ant & currentAnt : ant)
	{
		if (currentAnt.active && currentAnt.groupID == id)
			antsFound.push_back(&currentAnt);
	}
	
	if (!antsFound.empty() )
	{
		
		if (selectedAnt != antsFound)
		{
			selectedAnt = antsFound;
			play_sound(s_reporting1);
		}
		
		else // repetindo selecao? centraliza
		{
			
			/*
			Vector2 point;
			
			for (Ant * currentAnt : selectedAnt)
			{
				point += currentAnt->position;
			}
			point /= (float)selectedAnt.size();
			
			camera.move_to(point.x, point.y);
			*/
			
			camera.move_to(selectedAnt[0]->position.x, selectedAnt[0]->position.y);
			
		}
		
	}
	
}


void GameManager::select_all_ants()
{
	
	if (!localPlayer) return;
	
	std::vector<Ant*> antsFound;
	for (Ant & currentAnt : ant)
	{
		if (currentAnt.active && currentAnt.owner == localPlayer)
			antsFound.push_back(&currentAnt);
	}
	
	
	if (!antsFound.empty() )
	{
		
		if (selectedAnt != antsFound)
		{
			selectedAnt = antsFound;
			play_sound(s_reporting1);
		}
		
		else // repetindo selecao? centraliza
		{
			camera.move_to(selectedAnt[0]->position.x, selectedAnt[0]->position.y);
		}
		
	}
	
}
