//#include "NetController.h"
//NetController netController;

//#include <string>
//#include "lists.h"

#include "GameManager.h"
#include "net_commands_list.h"


void GameManager::Net_process_input()
{
	
	if (netController.is_online() )
	{
		
		while (netController.process_input_and_output() > 0)
		{
			
			if (netController.server_running() )
				Server_handle_net_event();
			
			else
				Client_handle_net_event();
			
		}
		
		/*
		if (netController.server_running() )
		{
			
			Server_listen_for_sockets();
			
			// manutencao do registro do server no master
			if (server_is_registered_at_master)
			{
				
				server_alive_notification_delay--;
				if (server_alive_notification_delay <= 0)
				{
					//Server_register_to_master();
					al_run_detached_thread(register_to_master_thread, NULL);
					server_alive_notification_delay = SEND_ALIVE_NOTIFICATION_DELAY;
				}
				
			}
			
		}*/
	
	}
	
}


void GameManager::Net_process_output()
{
	netController.process_output();
}


void GameManager::Server_handle_net_event()
{
	
	const int event_type = netController.get_event_type();
	switch (event_type) 
	{
		
		case 1: //ENET_EVENT_TYPE_CONNECT: 
		{
			
			Server_handle_new_connection();
			break;
			
		}
		
		case 3: //ENET_EVENT_TYPE_RECEIVE:
		{
			//printf("Packet received\n");
			Server_handle_received_packet();
			break;
		}
		   
		case 2: //ENET_EVENT_TYPE_DISCONNECT:
		{
			Server_handle_disconnection_event();
			printf("Client disconnected\n");
			break;			
		}
		
		default:
			break;
		
	}
	
}


void GameManager::Client_handle_net_event()
{
	
	const int event_type = netController.get_event_type();
	switch (event_type) 
	{
		
		case 1: //ENET_EVENT_TYPE_CONNECT: 
		{
			// nada
			break;
		}
		
		case 3: //ENET_EVENT_TYPE_RECEIVE:
		{
			//printf("Packet received\n");
			Client_handle_received_packet();
			break;
		}
		
		case 2: //ENET_EVENT_TYPE_DISCONNECT:
		{
			
			// desconexao anormal, sem a intencao do cliente
			/*
			if (game_state == GAMESTATE::MENU)
			{
				
				if (Minimap.active) Minimap.destroy();
				if (Console.visible) Console.hide();
				Map.loaded = false; // forca recarregamento na proxima
				
				LocalPlayer = NULL;
				Player.clear();
				
			}
			
			else if (game_state == GAMESTATE::PLAYING)
				GameEnded = true;*/
			
			const int disconnection_code = netController.client_handle_unexpected_disconnection();
			bool connection_lost = false;
			(void)connection_lost;
			
			std::string message = std::to_string(disconnection_code);
			
			
			switch (disconnection_code)
			{
				
				case DISCONNECTION::SERVER_CLOSING:
					message = "Server closed"; break;
					
				case DISCONNECTION::KICKED:
					message = "You have been kicked from the server!"; break;
					
				case DISCONNECTION::INCORRECT_PASSWORD:
					message = "Incorrect password"; break;
					
				case DISCONNECTION::SERVER_IS_FULL:
					message = "Sorry, server is full"; break;
					
				case DISCONNECTION::GAME_ALREADY_STARTED:
					message = "Sorry, game already started"; break;
					
				case DISCONNECTION::INCORRECT_VERSION:
					message = "Server is running a different version"; break;
				
				case DISCONNECTION::LOGIN_ERROR:
					message = "Incorrect name or password"; break;
					
				case DISCONNECTION::ALREADY_IN:
					message = "Player already ingame"; break;
					
				default:
				{
					connection_lost = true;
					message = "Lost connection to the server!";
					//DEBUG_PRINTF("Disconnection code: %d", disconnection_code);
				}
				
			}
			
			/*
			if (connection_lost)
				connection_lost_window();
			else
				info_window(message.c_str() );
			*/
			add_message_to_the_console(message);
			
			//printf("%s\n", message.c_str() );
			
			break;
			
		}
		
		default:
			printf("Received enet event %d\n", event_type);
			break;
		
	}
	
}



void GameManager::Server_handle_new_connection()
{
	
	bool error = false;
	int error_code = 0;
	std::string message;
	
	if (!gameInProgress)
	{
		
		// autenticacao basica
		bool should_accept_client;
		bool already_connected = false;
		
		// steam ja faz a autenticacao por si so, aceitar sempre
		#ifdef _STEAM_MULTIPLAYER
		if (netController.is_using_steam_lobby() )
		{
			//should_accept_client = true;
			already_connected = netController.steam_event_peer_already_on_lobby();
			should_accept_client = !already_connected;
		}
		else
		#endif
		should_accept_client = netController.get_event_data() == GAMECODE;
		
		if (should_accept_client)
		{
			
			if (netController.has_room_for_new_client() )
			{
				
				netController.add_event_remote_client();
				
				if (allow_debug)
				{
					
					#ifdef _STEAM_MULTIPLAYER
					if (netController.is_using_steam_lobby() )
						message = "New client connected from Steam";
					else
					#endif
					{
						message = "New client connected from ";
						
						// descobre o endereco do cliente
						std::string ip_string = netController.get_event_peer_ip_address();
						if (!ip_string.empty() )
						{
							
							message += ip_string; message += ":"; 
							message += to_stringi( (int)netController.get_event_peer_port() );
							
						}
						else
							message += "(error getting ip)";
					}
					
				}
				else message = "New client connected!";
				
			}
			
			else
			{
				
				error = true;
				error_code = DISCONNECTION::SERVER_IS_FULL;
				message = "Someone tried to join, but server is full.";
				
			}
			
		}
		
		else
		{
			
			error = true;
			error_code = DISCONNECTION::NORMAL;
			if (already_connected)
				message = "A client tried to join, but it is already connected.";
			else
				message = "Unknown client tried to connect. The connection was tossed into the cold void.";
			
		}
	
	}
	
	else
	{
		
		error = true;
		error_code = DISCONNECTION::GAME_ALREADY_STARTED;
		message = "Someone tried to join, but the game is already running.";
		
	}
	
	if (error)
	{
		netController.event_peer_disconnect_now(error_code);
	}
	
	//add_debug_message_to_console(message);
	printf("%s\n", message.c_str() );
	
}



void GameManager::Server_handle_disconnection_event()
{
	
	const int peer_id = netController.get_event_peer_incoming_peer_id();
	
	//string message = "Peer id "; message += to_stringi(peer_id); message += " disconnected!";
	netController.remove_remote_client(peer_id);	
	
	if (Player * target_player = get_Player_by_connection_id(peer_id) )
	{
		
		add_message_to_the_console("Player " + target_player->name + " disconnected", NULL);
		
		target_player->connection_id = NO_PLAYER_CONNECTION_ID;
		
		/*
		if (LongTurn && GameInProgress)
			notice_player_disconnection(*current_player);
		else
			remove_leaving_player(*current_player);
		*/
		
		communicate_player_disconnection(*target_player);
		
		if (!gameInProgress)
			remove_player(*target_player);
		
		play_sound(s_player_left);
		
	}
	else printf("Peer number %d disconnected. (no player)\n", peer_id);
		//add_debug_message_to_console("Peer number " + to_stringi(peer_id) + " disconnected. (no player)");
	
	
	netController.destroy_event_peer_data();
	
}


bool GameManager::start_server()
{
	
	if (netController.is_online() )
	{
		printf("Disconnect first!\n");
		return false;
	}
	
	bool HostAndPlay = true;
	
	bool success = false;
	#ifdef _STEAM_MULTIPLAYER
	if (use_steam_lobby)
		success = netController.start_steam_lobby(MAX_PLAYERS, steam_lobby_type, 2, HostAndPlay);
	else
	#endif
	success = netController.start_server(MAX_PLAYERS, port, 2, HostAndPlay);
	
	if (success)
	{
		
		printf("Hosting\n");
		
		/*
		bool use_listen_socket = true;
		#ifdef _STEAM_MULTIPLAYER
		if (use_steam_lobby)
			use_listen_socket = false;
		#endif
		
		if (use_listen_socket)
		{
			if (netController.start_listen_socket(LISTEN_PORT) )
			{
				
				add_message_to_console("Listening for scans on port " + to_stringi(LISTEN_PORT) );
				
				if (broadcast_to_master)
					al_run_detached_thread(register_to_master_thread, NULL);
				
			}
		}
		
		ServerName = ServerNameLocal;
		if (ServerName.empty() ) 
			ServerName = "Unnamed Server";
		
		// corrige possivel discrepancia
		if (allow_server_admin && HostAndPlay)
			allow_server_admin = false;
		*/
		
		/*
		if (!SaveFileName.empty())
		{
			if (!load_game(SaveFileName, true) )
			{
				
				netController.disconnect();
			}
		}
		else
		{
			
			GameMode = GAMEMODE::NORMAL;
			do_basic_player_setup();
			game_setup_window();
			
		}*/
		
		
		#ifdef _STEAM_MULTIPLAYER
		// seta informacoes do lobby
		if (use_steam_lobby)
		{
			
			netController.set_lobby_data("Name", ServerName.c_str() );
			netController.set_lobby_data("Map", Map.loaded ? Map.get_name() : selected_map_name.c_str() );
			netController.set_lobby_data("Pass", ServerPassword.empty() ? "0" : "1");
			netController.set_lobby_data("Dedicated", HostAndPlay ? "0" : "1");
			
		}
		#endif
		
	}
	// erros sao tratados dentro do netcontroller
	
	return success;
	
}




bool GameManager::attempt_to_connect_to_server(std::string ip, int port)
{
	
	bool success = false;
	
	std::string address_string;
	if (ip.empty() )
		address_string = "localhost:" + std::to_string(port);
	else
		address_string = ip + ":" + std::to_string(port);
	
	// mostra mensagem simples enquanto esta travado conectando
	std::string str = std::string( std::string("Connecting to ") + address_string + " ..." );
	printf("%s\n", str.c_str() );
	/*
	show_simple_window(str);
	draw();
	al_flip_display();
	*/
	
	if (netController.connect_to_server(ip.c_str(), port, 2, GAMECODE) )
	{
		
		printf("Connected!");
		
		/*
		// conectou!
		lastJoinedServer = server_info(ip, port);
		simple_window_time = 0.0f; // remove a janelinha
		
		GameMode = GAMEMODE::NORMAL;
		do_basic_player_setup();
		just_connected_window();
		//game_setup_window();
		
		*/
		// manda suas informacoes
		Client_send_join_authentication();
		
		success = true;
		
	}
	
	// falhou a conexao
	else 
	{
		printf("%s\n", netController.last_message.c_str() );
		//show_simple_window(netController.last_message.c_str() );
	}
	
	return success;
	
}


/*
void GameManager::Server_handle_new_connection()
{
	
	printf("Client connected\n");
	
	bool error = false;
	int error_code = 0;
	std::string message;
	
	bool GameInProgress = gameState == GAMESTATE::PLAYING;
	if (!GameInProgress)
	{
		
		// autenticacao basica
		bool should_accept_client = true;
		bool already_connected = false;
		
		// steam ja faz a autenticacao por si so, aceitar sempre
		#ifdef _STEAM_MULTIPLAYER
		if (NetController.is_using_steam_lobby() )
		{
			//should_accept_client = true;
			already_connected = NetController.steam_event_peer_already_on_lobby();
			should_accept_client = !already_connected;
		}
		else
		#endif
		//should_accept_client = NetController.get_event_data() == GAMECODE;
		
		if (should_accept_client)
		{
			
			if (netController.has_room_for_new_client() )
			{
				
				netController.add_event_remote_client();
				
			}
			
			else
			{
				
				error = true;
				error_code = DISCONNECTION::SERVER_IS_FULL;
				message = "Someone tried to join, but server is full.";
				
			}
			
		}
		
		else
		{
			
			error = true;
			error_code = DISCONNECTION::NORMAL;
			if (already_connected)
				message = "A client tried to join, but it is already connected.";
			else
				message = "Unknown client tried to connect. The connection was tossed into the cold void.";
			
		}
	
	}
	
	else
	{
		
		error = true;
		error_code = DISCONNECTION::GAME_ALREADY_STARTED;
		message = "Someone tried to join, but the game is already running.";
		
	}
	
	if (error)
	{
		netController.event_peer_disconnect_now(error_code);
	}
	
	printf("%s\n", message.c_str() );
	
}*/

