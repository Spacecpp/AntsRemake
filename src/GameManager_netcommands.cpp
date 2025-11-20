#include "GameManager.h"
#include "net_commands_list.h"
#include "Colors.h"

void GameManager::Client_handle_received_packet()
{
	
	//BitStream _data;
	//get_event_packet(_data);
	BitStream _data(netController.get_event_packet_data(), netController.get_event_packet_datalength() );
	
	int op = _data.getInt(bytesOf(OP::COUNT) );
	
	switch (op)
	{
		
		case OP::JOINING:
		{
			
			int8_t my_id = _data.getInt(1);
			unpack_gamestate(_data, my_id);
			
			play_sound(s_connect);
			
			break;
			
		}
		
		case OP::PLAYER_DISCONNECTION:
		{
			
			int8_t player_id = _data.getInt(1);
			
			if (Player * target_player = get_Player_by_id(player_id) )
			{
				
				add_message_to_the_console("Player " + target_player->name + " disconnected", NULL);
				
				if (!gameInProgress)
					remove_player(*target_player);
				
				play_sound(s_player_left);
				
				//else printf("Player %s disconnected\n", target_player->name.c_str() );
			}
			
			break;
			
		}
		
		case OP::ADD_PLAYER:
		{
			
			int8_t player_id = _data.getInt(1);
			std::string player_name = _data.getString();
			int8_t colorIndex = _data.getInt(1);
			/*
			ColorRGBA color;
			color.r = ( (float)_data.getInt(1) / 255.0f);
			color.g = ( (float)_data.getInt(1) / 255.0f);
			color.b = ( (float)_data.getInt(1) / 255.0f);
			color.a = 1.0f;*/
			
			int8_t controller = CONTROLLER::REMOTE;
			
			add_player( Player(player_name, colorIndex, controller, player_id, 0) );
			add_message_to_the_console("Player " + player_name + " connected!", NULL);
			
			play_sound(s_connect);
			
			break;
			
		}
		
		case OP::GAME_START:
		{
			
			uint32_t seed = _data.getUInt32();
			gameRNG.set_seed(seed);
			//printf("Received seed %u\n", seed);
			
			attempt_to_start_game();
			break;
			
		}
		
		case OP::CHAT_MESSAGE:
		{
			read_chat_message_packet(_data);
			break;
		}
		
		case OP::UPDATE:
		{
			read_update_packet(_data, NULL);
			break;
		}
		
		case OP::RESYNC:
		{
			unpack_resync(_data);
			add_message_to_the_console("Resync received");
			break;
		}
		
		case OP::MAP_CHANGE:
		{
			
			if (!gameInProgress)
			{
				
				String oldMapName = selectedMapName;
				selectedMapName = _data.getString();
				
				// refresh
				if (oldMapName != selectedMapName)
					load_selected_map();
				
				if (_GUI_Window.active)
					_GUI_Window.refresh();
			}
			else
				printf("Tried to change map during game\n");
			
			break;
			
		}
		
		case OP::RULE_UPDATE:
		{
			
			commandDelay = _data.getUInt32();
			
			if (_GUI_Window.active)
				_GUI_Window.refresh();
			
			break;
			
		}
		
		case OP::COLOR_CHANGE:
		{
			
			int8_t id = _data.getInt8();
			if (Player * targetPlayer = get_Player_by_id(id) )
			{
				int8_t colorIndex = _data.getInt8();
				targetPlayer->change_color(colorIndex);
			}
			
			break;
			
		}
		
		case OP::READY_TICK:
		{
			
			int8_t id = _data.getInt8();
			if (Player * targetPlayer = get_Player_by_id(id) )
			{
				bool ready = _data.getBool();
				targetPlayer->set_ready(ready);
			}
			
			break;
			
		}
		
		
		
		
		
		/*
		case OP::ANT_MOVE:
		{
			read_ant_move_packet(_data);
			break;
		}
		
		case OP::ANT_ATTACK:
		{
			read_ant_attack_packet(_data);
			break;
		}
		
		case OP::ANT_ABILITY:
		{
			read_ant_ability_use_packet(_data);
			break;
		}
		
		case OP::EGG_HATCH:
		{
			read_egg_hatch_packet(_data);
			break;
		}
		*/
		
		default:
			printf("Received OP %d\n", op);
		
	}
	
}



void GameManager::Server_handle_received_packet()
{
	
	//BitStream _data;
	//get_event_packet(_data);
	
	//int peer_id = netController.get_event_peer_incoming_peer_id();
	BitStream _data(netController.get_event_packet_data(), netController.get_event_packet_datalength() );
	
	int op = _data.getInt(bytesOf(OP::COUNT) );
	
	switch (op)
	{
		
		case OP::JOINING:
		{
			
			std::string version_string = _data.getString();
			
			if (version_string == VERSION_STRING)
            {
                
                std::string new_player_name = _data.getString();
                
                int new_player_id = playerNextUniqueID++;
                const int index = get_next_player_free_index();
                int8_t colorIndex = index;
                //ColorRGBA new_player_color = get_indexed_color(index);
                
                const int peer_id = netController.get_event_peer_incoming_peer_id();
                
                add_player( Player(new_player_name, colorIndex, CONTROLLER::REMOTE, new_player_id, peer_id) );
                add_message_to_the_console("Player " + new_player_name + " connected!", NULL);
                
                // devolve a lista de players e regras do jogo ao jogador
                {
                    
                    BitStream data;
                    data.addInt(OP::JOINING, bytesOf(OP::COUNT) );
                    
                    // unique id que o jogador pegou
                    data.addInt(new_player_id, 1);
                    
                    pack_gamestate(data);
                    
                    netController.send_packet_to_event_peer(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
                    
                }
                
                // se houver + de 1 cliente, comunicar a entrada desse novo jogador
                if (netController.get_number_of_users_connected() > 1)
                {
                    
                    BitStream data;
                    data.addInt(OP::ADD_PLAYER, bytesOf(OP::COUNT) );
                    
                    data.addInt(new_player_id, 1);
                    data.addString(new_player_name);
                    data.addInt(colorIndex, 1);
                    /*
                    data.addInt( (int)new_player_color.r * 255, 1);
                    data.addInt( (int)new_player_color.g * 255, 1);
                    data.addInt( (int)new_player_color.b * 255, 1);
                    */
                    
                    netController.send_packet_to_all_clients_except_event_peer(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
                    
                }
                
            }
            
            // versao incorreta
            else
            {
                
                std::string str = "Client connected using a different version (" + version_string + ").";
                add_message_to_the_console(str);
                netController.event_peer_disconnect_now(DISCONNECTION::INCORRECT_VERSION);
                
            }
			
			break;
			
		}
		
		case OP::CHAT_MESSAGE:
		{
			read_chat_message_packet(_data);
			break;
		}
		
		case OP::UPDATE:
		{
			
			unsigned int peer_id = netController.get_event_peer_incoming_peer_id();
			Player * targetPlayer = get_Player_by_connection_id(peer_id);
			
			if (!targetPlayer)
				printf("Player id %d not found for update packet\n", peer_id);
			
			read_update_packet(_data, targetPlayer);
			
			break;
			
		}
		
		case OP::RESYNC:
		{
			pendingResync = true;
			break;
		}
		
		case OP::COLOR_CHANGE:
		{
			
			unsigned int peer_id = netController.get_event_peer_incoming_peer_id();
			if (Player * clientPlayer = get_Player_by_connection_id(peer_id) )
			{
			
				int8_t id = _data.getInt8();
				if (Player * targetPlayer = get_Player_by_id(id) )
				{
					
					if (clientPlayer == targetPlayer)
					{
					
						int8_t colorIndex = _data.getInt8();
						targetPlayer->change_color(colorIndex);
						
						// repassa
						comunicate_color_change(*targetPlayer);
						
					}
					
				}
			
			}
			
			break;
			
		}
		
		case OP::READY_TICK:
		{
			
			unsigned int peer_id = netController.get_event_peer_incoming_peer_id();
			if (Player * clientPlayer = get_Player_by_connection_id(peer_id) )
			{
			
				int8_t id = _data.getInt8();
				if (Player * targetPlayer = get_Player_by_id(id) )
				{
					
					if (clientPlayer == targetPlayer)
					{
					
						bool ready = _data.getBool();
						targetPlayer->set_ready(ready);
						
						// repassa
						comunicate_ready_tick(*targetPlayer);
						
					}
					
				}
			
			}
			
			break;
			
		}
		
		/*
		case OP::ANT_MOVE:
		{
			read_ant_move_packet(_data);
			break;
		}
		
		case OP::ANT_ATTACK:
		{
			read_ant_attack_packet(_data);
			break;
		}
		
		case OP::ANT_ABILITY:
		{
			read_ant_ability_use_packet(_data);
			break;
		}
		
		case OP::EGG_HATCH:
		{
			read_egg_hatch_packet(_data);
			break;
		}
		*/
		default:
			printf("Received OP %d\n", op);
		
	}
	
}



void GameManager::Client_send_join_authentication()
{
	
	if (!netController.is_client() ) return;
	
	BitStream data;
	data.addInt(OP::JOINING, bytesOf(OP::COUNT) );
	
	data.addString(VERSION_STRING);
	//data.addBool(IS_DEMO);
	
	//data.addString(ServerPasswordConnect);
	data.addString(localPlayerName);
	//data.addString(PlayerProfile.password);
	
	// primeiro verifica se a cor atual eh compativel
	/*
	ALLEGRO_COLOR color = PlayerProfile.color;
	assert_player_color_availability(&color);
	
	unsigned char r, g, b;
	al_unmap_rgb(color, &r, &g, &b);
	
	data.addInt( (int)(r), bytesOf(255) );
	data.addInt( (int)(g), bytesOf(255) );
	data.addInt( (int)(b), bytesOf(255) );
	
	data.addUChar(PlayerProfile.avatar_id);
	*/
	
	netController.send_packet_to_server(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
	
	//DEBUG_PRINTF("Authentication Sent\n");
	
}



void GameManager::Server_comunicate_game_start()
{
	
	BitStream data;
	data.addInt(OP::GAME_START, bytesOf(OP::COUNT) );
	
	uint32_t seed = gameRNG.get_state_as_integer();
	data.addUInt32(seed);
	//printf("Sending seed %u\n", seed);
	
	netController.send_packet_to_all_clients(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
	
}


/*
void GameManager::send_packet_to_all(BitStream & data, int channel_id, uint32_t packet_flags)
{
	
	if (netController.server_running() )
		netController.send_packet_to_all_clients(data.getData(), data.getDataSize(), channel_id, packet_flags);
	else
		netController.send_packet_to_server(data.getData(), data.getDataSize(), channel_id, packet_flags);
	
}*/



void GameManager::communicate_player_disconnection(Player & targetPlayer)
{
	
	BitStream data;
	data.addInt(OP::PLAYER_DISCONNECTION, bytesOf(OP::COUNT) );
	
	data.addInt(targetPlayer.id, 1);
	
	//send_packet_to_all(data, 1, PACKET_FLAG_RELIABLE);
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}



void GameManager::communicate_chat_message(const std::string & message, Player * sender)
{
	
	BitStream data;
	data.addInt(OP::CHAT_MESSAGE, bytesOf(OP::COUNT) );
	
	data.addString(message);
	
	if (sender)
		data.addInt(sender->id, 1);
	else
		data.addInt(128, 1);
	
	//send_packet_to_all(data, 1, PACKET_FLAG_RELIABLE);
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}


void GameManager::communicate_map_change()
{
	
	BitStream data;
	data.addInt(OP::MAP_CHANGE, bytesOf(OP::COUNT) );
	
	data.addString(selectedMapName);
	
	//send_packet_to_all(data, 1, PACKET_FLAG_RELIABLE);
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}



void GameManager::comunicate_rule_update()
{
	
	BitStream data;
	data.addInt(OP::RULE_UPDATE, bytesOf(OP::COUNT) );
	
	data.addUInt32(commandDelay);
	
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}



void GameManager::communicate_update()
{
	
	//const bool is_server = netController.server_running();
	//const bool is_client = !is_server;
	
	BitStream data;
	data.addInt(OP::UPDATE, bytesOf(OP::COUNT) );
	
	//data.addUInt32(gameFrame);
	data.addUInt32(gameFrameMax);
	
	const int order_count = netQueuedOrder.size();
	data.addInt(order_count, 2);
	
	for (GameOrder & current_order : netQueuedOrder)
		current_order.pack(data);
	
	//send_packet_to_all(data, 1, PACKET_FLAG_RELIABLE);
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}


void GameManager::comunicate_color_change(Player & targetPlayer)
{
	
	BitStream data;
	data.addInt(OP::COLOR_CHANGE, bytesOf(OP::COUNT) );
	data.addInt8(targetPlayer.id);
	data.addInt8(targetPlayer.colorIndex);
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}



void GameManager::comunicate_ready_tick(Player & targetPlayer)
{
	
	BitStream data;
	data.addInt(OP::READY_TICK, bytesOf(OP::COUNT) );
	data.addInt8(targetPlayer.id);
	data.addBool(targetPlayer.ready);
	netController.send_packet(data.getData(), data.getDataSize(), 1, PACKET_FLAG_RELIABLE);
	
}


/*
void GameManager::communicate_ant_move(std::vector<Ant*> & targetAnts, int x, int y, bool goingAfterFood)
{
	
	BitStream data;
	data.addInt(OP::ANT_MOVE, bytesOf(OP::COUNT) );
	
	// coloca a quantidade de formigas, seguido de seus ids
	data.addInt( (int)targetAnts.size(), 1);
	for (Ant * current_ant : targetAnts)
	{
		data.addInt16(current_ant->unique_id);
	}
	
	// coordenadas
	data.addInt(x, 2);
	data.addInt(y, 2);
	
	data.addBool(goingAfterFood);
	
	
	// envia
	if (netController.server_running() )
		netController.send_packet_to_all_clients(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
	else
		netController.send_packet_to_server(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
	
}



void GameManager::communicate_ant_attack( std::vector<Ant*> & targetAnts, Ant & defenderAnt)
{
	
	BitStream data;
	data.addInt(OP::ANT_ATTACK, bytesOf(OP::COUNT) );
	
	// coloca a quantidade de formigas, seguido de seus ids
	data.addInt( (int)targetAnts.size(), 1);
	for (Ant * current_ant : targetAnts)
	{
		data.addInt16(current_ant->unique_id);
	}
	
	// alvo
	data.addInt16(defenderAnt.unique_id);
	
	
	// envia
	if (netController.server_running() )
		netController.send_packet_to_all_clients(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
	else
		netController.send_packet_to_server(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
	
}



void GameManager::communicate_ant_ability_use(Ant & targetAnt, int x, int y, int8_t option)
{
	
	BitStream data;
	data.addInt(OP::ANT_ABILITY, bytesOf(OP::COUNT) );
	
	data.addInt16(targetAnt.unique_id);
	
	data.addInt(x, 2);
	data.addInt(y, 2);
	
	data.addInt16(option);
	
	send_packet_to_all(data, 0, PACKET_FLAG_RELIABLE);
	
}



void GameManager::communicate_egg_hatch(AntHill & targetAntHill)
{
	
	if (targetAntHill.owner == NULL) return;
	
	BitStream data;
	data.addInt(OP::EGG_HATCH, bytesOf(OP::COUNT) );
	
	data.addInt(targetAntHill.owner->id, 1);
	
	send_packet_to_all(data, 0, PACKET_FLAG_RELIABLE);
	
}
*/

//---------


void GameManager::read_chat_message_packet(BitStream & _data)
{
	
	std::string message = _data.getString();
	
	int player_id = _data.getInt(1);
	Player * sender = get_Player_by_id(player_id);
	
	add_chat_message_to_the_console(message, sender);
	
	// repassa
	if (netController.server_running() )
		communicate_chat_message(message, sender);
	
}



void GameManager::read_update_packet(BitStream & _data, Player * senderPlayer)
{
	
	const bool is_server = netController.server_running();
	const bool is_client = !is_server;
	
	//const bool should_add_delay = (is_server && networkFrame > gameFrameMax);
	
	// cliente manda pra fila do jogo imediatamente. servidor precisa repassar pros clientes primeiro
	std::list<GameOrder> & target_queue = is_client ? queuedOrder : netQueuedOrder;
	
	uint32_t received_gameFrame = _data.getUInt32();
	
	int order_count = _data.getInt(2);
	for (int i = 0; i < order_count; i++)
	{
		
		GameOrder order;
		order.unpack(_data);
		
		//if (should_add_delay)
        //   order.targetGameFrame += commandDelay;
        
        // adiciona delay extra se necessario
        if (is_server)
        {
            
            if (order.targetGameFrame <= gameFrameMax)
                order.targetGameFrame = gameFrameMax + commandDelay + 1;
            
            else if (order.targetGameFrame <= gameFrameMax + commandDelay)
                order.targetGameFrame += commandDelay;
        }
		
		target_queue.push_back(order);
		
	}
	
	if (is_server)
	{
		if (senderPlayer)
			senderPlayer->gameFrame = received_gameFrame;
	}
	
	else // client
	{
		gameFrameMax = received_gameFrame + commandDelay;
	}
	
	//printf("%d\n", order_count);
	
}


/*
void GameManager::read_ant_move_packet(BitStream & _data)
{
	
	std::vector<Ant*> targetAnts;
	
	int ant_count = _data.getInt(1);
	for (int i = 0; i < ant_count; i++)
	{
		
		int16_t ant_id = _data.getInt16();
		
		if (Ant * current_ant = get_Ant(ant_id) )
			targetAnts.push_back(current_ant);
		
	}
	
	int x = _data.getInt(2); 
	int y = _data.getInt(2); 
	
	bool goingAfterFood = _data.getBool();
	
	order_ant_move(true, targetAnts, x, y, goingAfterFood);
	
}



void GameManager::read_ant_attack_packet(BitStream & _data)
{
	
	std::vector<Ant*> targetAnts;
	
	int ant_count = _data.getInt(1);
	for (int i = 0; i < ant_count; i++)
	{
		
		int16_t ant_id = _data.getInt16();
		
		if (Ant * current_ant = get_Ant(ant_id) )
			targetAnts.push_back(current_ant);
		
	}
	
	int16_t defender_id = _data.getInt16();
	if (Ant * defenderAnt = get_Ant(defender_id) )
		order_ant_attack(true, targetAnts, *defenderAnt);
	
}



void GameManager::read_ant_ability_use_packet(BitStream & _data)
{
	
	int16_t ant_id = _data.getInt16();
	
	int x = _data.getInt(2); 
	int y = _data.getInt(2); 
	
	int16_t option = _data.getInt16();
	
	if (Ant * targetAnt = get_Ant(ant_id) )
		order_ant_use_special_ability(true, *targetAnt, x, y, option);
	
}



void GameManager::read_egg_hatch_packet(BitStream & _data)
{
	
	int player_id = _data.getInt(1);
	
	if (AntHill * targetAntHill = get_AntHill(player_id) )
		targetAntHill->hatch_egg();
	
}
*/


void GameManager::pack_gamestate(BitStream & data)
{
	
	int player_count = 0;
	for (Player & c_Player : player)
	{
		if (c_Player.active)
			player_count++;
	}
	
	data.addInt(player_count, 1);
	for (Player & c_Player : player)
	{
		if (c_Player.active)
		{
			
			data.addInt(c_Player.id, 1);
			data.addString(c_Player.name);
			data.addInt(c_Player.colorIndex, 1);
			/*
			unsigned char r, g, b, a;
			color_float_to_unsigned_char(c_Player.color, r, g, b, a);
			
			data.addInt(r, 1);
			data.addInt(g, 1);
			data.addInt(b, 1);
			*/
			data.addBool(c_Player.ready);
			
		}
	}
	
	data.addString(selectedMapName);
	data.addUInt(commandDelay, 2);
	
}



void GameManager::unpack_gamestate(BitStream & _data, int8_t my_id)
{
	
	player.clear();
	player.reserve(MAX_PLAYERS);
	playerNextUniqueID = 0;
	
	inGameSetupScreen = true;
	
	int player_count = _data.getInt(1);
	for (int i = 0; i < player_count; i++)
	{
		
		int8_t player_id = _data.getInt(1);
		std::string player_name = _data.getString();
		int8_t colorIndex = _data.getInt(1);
		/*
		Color32 color;
		color.r = _data.getInt(1);
		color.g = _data.getInt(1);
		color.b = _data.getInt(1);
		color.a = 255;*/
		
		bool ready = _data.getBool();
		
		int8_t controller = player_id == my_id ? CONTROLLER::LOCAL : CONTROLLER::REMOTE;
		
		if (Player * p = add_player( Player(player_name, colorIndex, controller, player_id, 0) ) )
		{
			p->ready = ready;
		}
		
	}
	
	selectedMapName = _data.getString();
	commandDelay = _data.getUInt(2);
	
	find_local_player();
	
	game_setup_window();
	
}


void GameManager::pack_resync(BitStream & data)
{
	
	uint32_t seed = gameRNG.get_state_as_integer();
	data.addUInt32(seed);
    
    data.addUInt(gameFrame, 4);
    //antNextUniqueID
    
    // players
    for (Player & c_Player : player) 
    {
        if (c_Player.active)
        {
            c_Player.pack_resync_data(data);
        }
    }
    
    // ants
    data.addInt(get_ant_count(), 2);
    for (Ant & c_ant : ant) 
    {
        if (c_ant.active)
            c_ant.pack_data(data);
    }
    
    // powerup
    data.addInt(get_powerup_count(), 2);
    for (Powerup & c_powerup : powerup) 
    {
        if (c_powerup.active)
            c_powerup.pack_data(data);
    }
    
    // food
    data.addInt(food.size(), 2);
    for (Food & c_food : food) 
    {
        c_food.pack_resync_data(data);
    }
    
    
    // objetos que fazem parte do gameMap
    std::vector<IntVector3> map_objects;
    for (int y = 0; y < gameMap.height; y++) 
	{
		for (int x = 0; x < gameMap.width; x++) 
		{
			
			const int8_t tile_object = gameMap.terrain[y][x].secondary;
			if (tile_object != SECONDARY_TILE::NONE)
            {
                map_objects.push_back( IntVector3(x, y, tile_object) );
            }
		}
		
	}
	
	data.addUInt(map_objects.size(), 4);
	for (IntVector3 & current : map_objects)
    {
        data.addUInt(current.x, 2);
        data.addUInt(current.y, 2);
        data.addUInt(current.z, 1);
    }
    // ----
    
}


void GameManager::unpack_resync(BitStream & _data)
{
	
	uint32_t seed = _data.getUInt32();
	gameRNG.set_seed(seed);
    
    gameFrame = _data.getUInt(4);
    gameFrameMax = gameFrame + commandDelay - 1;
    networkFrame = gameFrame + 1;
    gameFrameDone = false;
    
    // players
    for (Player & c_Player : player) 
    {
        if (c_Player.active)
        {
            c_Player.unpack_resync_data(_data);
        }
    }
    
    // ---- ants -----
    
    // backup da selecao por id
    std::vector<int16_t> selectedAnt_ID;
    if (!selectedAnt.empty() )
    {
        for (Ant * current : selectedAnt)
            selectedAnt_ID.push_back(current->unique_id);
        
        selectedAnt.clear();
    }
    
    printf("antes do sync:\n");
    ant[0].debug_printf();
    
    //ant.clear();
    size_t ant_count = _data.getInt(2);
    printf("Received %d ants\n", (int)ant_count);
    
    if (ant_count <= MAX_ANTS)
        ant.resize(ant_count);
    else
        printf("Too much ants: %d\n", (int)ant_count);
    
    /*
    for (size_t i = 0; i < ant_count; i++)
        ant[i].unpack_data(_data);
    */
    
    for (Ant & current : ant)
        current.unpack_data(_data);
    
    // restaura ponteiros
    printf("About to refresh pointers\n");
    for (Ant & current : ant)
    {
        if (current.active)
            current.refresh_pointers();
    }
    printf("Done\n");
    
    // restaura selecao
    if (!selectedAnt_ID.empty() )
        get_Ants_by_ids(selectedAnt, selectedAnt_ID);
    
    printf("apos sync:\n");
    ant[0].debug_printf();
    
    // -------
    
    // powerups
    powerup.clear();
    size_t powerup_count = _data.getInt(2);
    if (powerup_count <= MAX_POWERUPS)
        powerup.resize(powerup_count);
    else
        printf("Too much powerups: %d\n", (int)powerup_count);
    
    for (size_t i = 0; i < powerup_count; i++)
    {
        powerup[i].unpack_data(_data);
    }
    
    
    // food
    size_t food_count = _data.getInt(2);
    if (food_count <= MAX_FOOD && food.size() != food_count)
        food.resize(food_count);
        
    for (Food & c_food : food) 
    {
        c_food.unpack_resync_data(_data);
    }
    
    
    
    // objetos que fazem parte do gameMap
    gameMap.remove_all_secondary_objects();
    size_t map_object_count = _data.getUInt(4);
    
    if (map_object_count <= (size_t)(gameMap.width * gameMap.height) )
    {
        
        for (size_t i = 0; i < map_object_count; i++)
        {
            
            int x = 0, y = 0, id = 0;
            x = _data.getUInt(2);
            y = _data.getUInt(2);
            id = _data.getUInt(1);
            gameMap.set_secondary_object(x, y, id);
            
            
        }
    
    }
    else printf("Warning: inconsistent amount of secondary objects: %u\n", (unsigned int)map_object_count);
    // -----
    
    printf("Resync done\n");
    
}
