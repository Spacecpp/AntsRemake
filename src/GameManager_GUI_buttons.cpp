#include "GameManager.h"
#include "interface_list.h"
#include "net_commands_list.h"


void GameManager::compute_pressed_button(GUI_Button & c_Button)
{
	
	const int code = c_Button.code;
	const int secondary_code = c_Button.secondary_code;
	(void)secondary_code;
	
	switch (code)
	{
		
		case BUTTON_ID::EXIT:
		{
			#ifndef PLATFORM_WEB
				gameState = GAMESTATE::EXIT;
			#endif
			break;
		}
		
		case BUTTON_ID::OK:
		{
			_GUI_Window.destroy();
			break;
		}
		
		case BUTTON_ID::BACK:
		{
			escape_command();
			break;
		}
		
		case BUTTON_ID::SINGLE_PLAYER:
		{
			setup_single_player_game();
			break;
		}
		
		case BUTTON_ID::MAP_EDITOR:
		{
			enter_map_editor();
			break;
		}
		
		case BUTTON_ID::OPTIONS:
		{
			options_window();
			break;
		}
		
		case BUTTON_ID::READY:
		{
			
			if (localPlayer)
				localPlayer->ready = !localPlayer->ready;
			
			// TODO: comunicar a mudanca
			
			break;
		}
		
		case BUTTON_ID::ONLINE_MENU:
		{
			
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				online_window();
			#else
				info_window("Multiplayer not yet implemented for web build", true);
			#endif
			
			break;
			
		}
		
		case BUTTON_ID::HOST_GAME:
		{
			
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				if (start_server() )
				{
					setup_new_game();
					console.clear_messages();
					game_setup_window();
				}
			#endif
			
			break;
			
		}
		
		case BUTTON_ID::JOIN_GAME:
		{
			
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				//info_window("Connecting...");
				
				if (attempt_to_connect_to_server(joinIP, port) )
				{
					console.clear_messages();
					game_setup_window();
				}
				else 
					server_not_responding_window(); //info_window("Server not responding");
				
				break;
			#endif
			
		}
		
		case BUTTON_ID::LEAVE_GAME:
		{
			//escape_command();
			leave_game();
			break;
		}
		
		case BUTTON_ID::DISCONNECT:
		{
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
				netController.disconnect();
			#endif
			enter_main_menu();
			break;
		}
		
		case BUTTON_ID::START_GAME:
		{
			attempt_to_start_game();
			break;
		}
		
		case BUTTON_ID::ADD_BOT:
		{
			add_bot();
			break;
		}
		
		case BUTTON_ID::REMOVE_BOT:
		{
			remove_bot();
			break;
		}
		
		case BUTTON_ID::SELECT_MAP_WINDOW:
		{
			select_map_window();
			break;
		}
		
		case BUTTON_ID::SELECT_MAP:
		{
			
			if (!_GUI_Window.listbox.empty() )
			{
				
				String mapName = _GUI_Window.listbox[0].get_selected_file_name();
				if (!mapName.empty() )
				{
					
					if (editMode)
					{
						
						String full_path = "Maps/" + mapName + ".map";
						String result;
						if (load_map(full_path.c_str() ) )
						{
							result = String("Map ") + mapName + " loaded!";
							editorSaveFilename = mapName;
						}
						else
							result = String("Error loading") + mapName;
						
						info_window(result);
						
					}
					
					else
					{
						
						if (mapName != selectedMapName)
						{
							
							selectedMapName = mapName;
							
							//std::string mappath = "Maps/" + selectedMapName + ".map";
							//load_map(mappath.c_str() );
							
							#ifdef ONLINE_MULTIPLAYER_AVAILABLE
								communicate_map_change();
							#endif
							
						}
						
						game_setup_window();
						
					}
					
				}
				else printf("mapName empty\n");
				
			}
			else printf("No listbox\n");
			
			break;
			
		}
		
		// map editor
		case BUTTON_ID::EDITOR_NEW_MAP:
		{
			
			if (!gameMap.wasModified || (_GUI_Window.active && _GUI_Window.code == WINDOW_ID::EDITOR_NEW_MAP) )
			{
			
				antHill.clear();
				ant.clear();
				food.clear();
				obstacle.clear();
				powerup.clear();
				
				gameMap.reset();
				gameMap.change_size(20, 20);
				
				refresh_camera();
				center_camera();
				
				
				_GUI_Window.destroy();
				
			}
			else confirm_new_map_window();
			
			break;
			
		}
		
		case BUTTON_ID::EDITOR_SAVE_MAP_WINDOW:
		{
			editor_save_map_window();
			break;
		}
		
		/*
		case BUTTON_ID::EDITOR_SAVE_MAP_ASK:
		{
		
		
			break;
			
		}*/
		
		case BUTTON_ID::EDITOR_SAVE_MAP:
		{
		
			if (!editorSaveFilename.empty() )
			{
				
				String full_path = "Maps/" + editorSaveFilename + ".map";
				String result;
				if (save_map(full_path.c_str() ) )
					result = String("Map ") + editorSaveFilename + " saved!";
				else
					result = String("Error saving") + editorSaveFilename;
				
				info_window(result);
				
			}
			break;
			
		}
		
		case BUTTON_ID::EDITOR_CHANGE_MODE:
		{
			editorElementType = secondary_code;
			create_map_editor_tool_window(); //refresh
			break;
		}
		
		case BUTTON_ID::EDITOR_TYPE_ID:
		{
			editorTileType = secondary_code;
			break;
		}
		
		case BUTTON_ID::EDITOR_PLAYER_ID:
		{
			editorPlayerID = secondary_code;
			break;
		}
		
		case BUTTON_ID::EDITOR_PREV_PAGE:
		{
			if (editorPage > 0)
			{
				editorPage--;
				create_map_editor_tool_window(); // refresh
			}
			break;
		}
		
		case BUTTON_ID::EDITOR_NEXT_PAGE:
		{
			if (editorPage < 127)
			{
				editorPage++;
				create_map_editor_tool_window(); // refresh
			}
			break;
		}
		
		case BUTTON_ID::EDITOR_MAP_PROPERTIES_WINDOW:
		{
			editor_map_properties_window();
			break;
		}
		
		case BUTTON_ID::EDITOR_RESIZE_MAP_WINDOW:
		{
			editor_map_resize_window();
			break;
		}
		
		case BUTTON_ID::EDITOR_APPLY_MAP_RESIZE:
		{
			
			if (_GUI_Window.textBox.size() >= 4)
			{
				
				const int up = _GUI_Window.textBox[0].get_int_text();
				const int down = _GUI_Window.textBox[1].get_int_text();
				const int left = _GUI_Window.textBox[2].get_int_text();
				const int right = _GUI_Window.textBox[3].get_int_text();
				
				if (up != 0 || down != 0 || left != 0 || right != 0)
				{
					
					const int predicted_width = gameMap.width + left + right;
					const int predicted_height = gameMap.height + up + down;
					
					if (predicted_width > 0 && predicted_width <= MAP_DIMENSION_LIMIT &&
						predicted_height > 0 && predicted_height <= MAP_DIMENSION_LIMIT)
					{
						
						resize_map_in_all_directions(up, down, left, right);
						_GUI_Window.destroy();
					
					}
					
					else 
					{
						
						info_window( String("Map dimensions must range from 1 to ") + to_stringi(MAP_DIMENSION_LIMIT) );
						play_sound(s_error);
						printf("Error: invalid map size, width %d, height %d\n", predicted_width, predicted_height);
						
					}
				
				}
			
			}
			else printf("Error: expected 4 textbox\n");
			
			break;	
			
		}
		
		case BUTTON_ID::HELP_SCREEN:
		{
			game_help_window();
			break;
		}
		
	}
	
}

