#include "GameManager.h"
#include "interface_list.h"
#include "net_commands_list.h"


void GameManager::process_inputs()
{
	
	if (key_down[KEY_ESCAPE])
		escape_command();	
	
	if (window_close_button_pressed) // X
		gameState = GAMESTATE::EXIT;
	
	
	if (gameState == GAMESTATE::PLAYING)
	{
		
		if (editMode)
			process_editor_inputs();
		
		else if (!gameEnded)
			process_gameplay_inputs();
		
		
		// geral
		const float screen_width = screen_w;
		const float screen_height = screen_h;
		
		compute_mouse_camera_movement(0.0f, 0.0f, screen_width, screen_height);
		
		if (key_pressed[KEY_LEFT])
			camera.x -= GAME_CAMERA_SPEED;
		
		if (key_pressed[KEY_RIGHT])
			camera.x += GAME_CAMERA_SPEED;
		
		if (key_pressed[KEY_UP])
			camera.y -= GAME_CAMERA_SPEED;
		
		if (key_pressed[KEY_DOWN])
			camera.y += GAME_CAMERA_SPEED;
		
		// zoom
		if ( !(console.active && console.currently_visible() && console.mouse_inside() ) )
		{
			
			float wheelMove = mouse_wheel;   
			
			if (wheelMove > 0.0f)
			{
				camera.zoom *= 1.1f;
				if (camera.zoom > 6.0f)
					camera.zoom = 6.0f;
			}
			
			else if (wheelMove < 0.0f)
			{
				camera.zoom *= 0.9f;
				if (camera.zoom < 0.25f)
					camera.zoom = 0.25f;
			}
			
		}
		
		if (key_down[KEY_KEYPAD_0] && !_GUI_Window.active && !(console.active && console.currently_visible() && console.inputFocus) )
			camera.zoom = 1.0f;
		
		camera.constrain_position(0.0f, 0.0f, gameMap.width * TILE_SIZE, gameMap.height * TILE_SIZE);
		camera.update();
		
		//----
		
		/*
		if (!consoleOpen && !netController.is_online() )
		{
			if (key_down[KEY_E])
				editMode = !editMode;
		}*/
		
		
		// DEBUG KEYS
		#ifdef _DEBUG_MODE 
		if (key_down[KEY_F3])
			debug_showPaths = !debug_showPaths;
		
		if (key_down[KEY_F2])
			show_debug = !show_debug;
			
		if (key_down[KEY_F11])
			simulateConnectionLost = !simulateConnectionLost;	
			
		if (key_down[KEY_F7])
			end_game_window();
			
        #endif
			
        
        // resync manual
		if (key_down[KEY_F10])
		{
			
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
		    if (netController.is_online() )
            {
                
                if (netController.is_client() )
                {
                    
                    BitStream data;
                    data.addInt(OP::RESYNC, bytesOf(OP::COUNT) );
                    netController.send_packet_to_server(data.getData(), data.getDataSize(), 0, PACKET_FLAG_RELIABLE);
                    
                }
                else
                {
                    pendingResync = true;
                }
                
            }
            #endif
		}
		// ------------
		
	}
	
	/*
	if (console.visible || gameState == GAMESTATE::PLAYING)
	{
		
		// console
		if (key_down[KEY_ENTER]|| key_down[KEY_KEYPAD_ENTER])
		{
			
			if (console.visible)
			{
				
				if (!consoleInput.empty() )
				{
					
					if (!netController.is_client() )
						add_chat_message_to_the_console(consoleInput, localPlayer);
					
					if (netController.is_online() )
						communicate_chat_message(consoleInput, localPlayer);
					
					consoleInput.clear();
					
				}
				
				console.visible = false;
				if (console.visible && gameState == GAMESTATE::PLAYING)
				{
					console.always_opaque = false;
					console.opacity = 1.0f;
				}
				
			}
			else 
			{
				console.visible = true;
				console.always_opaque = true;
			}
		}
		
		if (console.visible)	
			read_text_input();
		
	}*/
	
	
	if (_GUI_Window.active)
	{
		
		int response = _GUI_Window.update();
		if (response > 0)
		{
			if (_GUI_Window.last_clicked_button)
				compute_pressed_button(*_GUI_Window.last_clicked_button);
		}
		
		// refresh da posicao
		if (screen_resized)
		{
			if (_GUI_Window.code == WINDOW_ID::GAME_SETUP)
				place_console();
		}
	
	}
	
	if (mapEditor_upperMenu.active)
	{
		
		int response = mapEditor_upperMenu.update(_GUI_Window.active);
		if (response > 0)
		{
			if (mapEditor_upperMenu.last_clicked_button)
				compute_pressed_button(*mapEditor_upperMenu.last_clicked_button);
		}
	
	}
	
	if (mapEditor_toolWindow.active)
	{
		
		int response = mapEditor_toolWindow.update(_GUI_Window.active);
		if (response > 0)
		{
			if (mapEditor_toolWindow.last_clicked_button)
				compute_pressed_button(*mapEditor_toolWindow.last_clicked_button);
		}
	
	}
	
	
	
	
}


void GameManager::process_gameplay_inputs()
{
	
	const bool apply_orders_now = is_offline();
	bool selectedThisFrame = false;
	
	// conserta inconsistencia
	if (!selectedAnt.empty() )
	{
		for (int i = (int)selectedAnt.size() - 1; i >= 0; i--) 
		{
			if (!selectedAnt[i]->active)
				selectedAnt.erase(selectedAnt.begin() + i);
		}
	}
	
	const bool ignore_clicks = mouse_over_UI();

	// input botao esquerdo
	if (mouse_left_pressed && !ignore_clicks) //&& !mouse_left_down )
	{
		
		Vector2 mouse_position = get_mouse_world_position(); //GetMousePosition();
		
		// comeca a demarcar a caixa
		if (!draggingBoxSelection)
		{
			
			draggingBoxSelection = true;
			
			boxSelection.x = mouse_position.x;
			boxSelection.y = mouse_position.y;
			boxSelection.z = 0.0f;
			boxSelection.w = 0.0f;
			
		}
		
		else // arrastando
		{
			
			boxSelection.z = mouse_position.x - boxSelection.x;
			boxSelection.w = mouse_position.y - boxSelection.y;
			
		}
		
	}
	
	if (mouse_left_released)
	{
		
		bool selected = false;
		
		// soltou o botao do mouse, tenta selecionar formigas
		if (draggingBoxSelection && boxSelection.z != 0.0f && boxSelection.w != 0.0f)
		{
			
			// seleciona formigas que estiverem dentro
			selectedAnt.clear();
			draggingBoxSelection = false;
			
			// corrige dimensoes negativas se aplicavel
			if (boxSelection.z < 0.0f)
			{
				boxSelection.z = -boxSelection.z;
				boxSelection.x -= boxSelection.z;
			}
			
			if (boxSelection.w < 0.0f)
			{
				boxSelection.w = -boxSelection.w;
				boxSelection.y -= boxSelection.w;
			}
			
			
			for (Ant & c_ant : ant) 
			{
				if (c_ant.active)
				{
					
					//if (rectangle_collision(boxSelection.x, boxSelection.y, boxSelection.width, boxSelection.height, 
					//	(float)(c_ant.x * TILE_SIZE), (float)(c_ant.y * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE) )
					if (c_ant.owner == localPlayer && 
						GameObject_inside_rectangle(boxSelection.x, boxSelection.y, boxSelection.z, boxSelection.w, c_ant) )
					{
						selectedAnt.push_back(&c_ant);
						selected = true;
					}
					
				}
			}
			
		}
		
		// clique normal, seleciona apenas uma
		else
		{
			
			Ant * antToSelect = NULL;
			draggingBoxSelection = false;
			Vector2 mouse_position = get_mouse_world_position(); //GetMousePosition();
			
			for (Ant & c_ant : ant) 
			{
				if (c_ant.active)
				{
					//if (point_to_rectangle_collision_topleft(mouse_position.x, mouse_position.y, 
					//	(float)(c_ant.x * TILE_SIZE), (float)(c_ant.y * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE) )
					if (c_ant.owner == localPlayer && 
						point_on_GameObject(mouse_position.x, mouse_position.y, c_ant) )
					{
						antToSelect = &c_ant;
						selected = true;
						break;
					}
					
				}
			}
			
			if (antToSelect)
            {
                selectedAnt.clear();
                selectedAnt.push_back(antToSelect);
            }
            
		}
		
		if (selected)
        {
			play_sound(s_reporting1);
			selectedThisFrame = true;
        }
	}
	
	
	// ------ da ordem as formigas selecionadas ------------
	bool ability_key = (key_pressed[KEY_LEFT_SHIFT] || key_pressed[KEY_LEFT_CONTROL]);
	
	if (!ignore_clicks && !selectedThisFrame && mouse_right_down && !ability_key)
	{
		
		if (!selectedAnt.empty() )
		{
			
			Vector2 destination = get_mouse_world_position(); //GetMousePosition();
			IntVector2 tile_destination(destination.x / TILE_SIZE, destination.y / TILE_SIZE);
			
			bool ok = false;
			
			// ataque
			Ant * clicked_ant = get_Ant_at_position(destination.x, destination.y);
			if (clicked_ant && clicked_ant->owner != localPlayer)
			{
				
				if (order_ant_attack(apply_orders_now, selectedAnt, *clicked_ant) )
				{
					
					ok = true;
					play_sound(s_attack1);
					
					// repassa o input online
					/*
					if (is_online() )
						communicate_ant_attack(selectedAnt, *clicked_ant);
					*/
					
				}
				//else play_sound(s_no_no);
				
			}
			
			// andar
			bool goingAfterFood = get_food_at_tile(tile_destination.x, tile_destination.y);
			
			if (!ok)
			{
				
				if (order_ant_move(apply_orders_now, selectedAnt, tile_destination.x, tile_destination.y, goingAfterFood) )
				{
					
					ok = true;
					play_sound(s_moving1);
					
					// repassa o input online
					/*
					if (is_online() )
						communicate_ant_move(selectedAnt, tile_destination.x, tile_destination.y, goingAfterFood);
					*/
					
				}
				else play_sound(s_no_no);
			}
		
		}
		
	}
	
	// habilidades especiais
	if (!ignore_clicks && mouse_right_down && ability_key)
	{
		
		if (selectedAnt.size() == 1 && selectedAnt[0]->have_a_field_special_ability() )
		{
			
			Vector2 destination = get_mouse_world_position(); 
			IntVector2 tile_destination(destination.x / TILE_SIZE, destination.y / TILE_SIZE);
			
			if (!gameMap.coordinates_out_of_bounds(tile_destination.x, tile_destination.y) )
			{
				
				Ant & targetAnt = *selectedAnt[0];
				
				int8_t option = 1;
				
				if (targetAnt.currentPowerup == POWERUP::FIREMAN)
				{
					const int8_t fire = gameMap.get_fire(tile_destination.x, tile_destination.y);
					option = fire ? 2 : 1;
				}
				
				else if (targetAnt.currentPowerup == POWERUP::BOMBER)
				{
					const int8_t landmine = gameMap.get_landmine(tile_destination.x, tile_destination.y);
					option = landmine ? 2 : 1;
				}
				
				else if (targetAnt.currentPowerup == POWERUP::DIVER)
				{
					const int8_t value = gameMap.terrain[tile_destination.y][tile_destination.x].value;
					option = value ? 2 : 1;
				}
				
				else if (targetAnt.currentPowerup == POWERUP::ROBBER)
				{
					
					printf("Robber ability input\n");
					
					AntHill * targetAnthill = get_AntHill_at_tile(tile_destination.x, tile_destination.y);
					if (targetAnthill && targetAnthill->owner != targetAnt.owner)
					{
						printf("Anthill found\n");
						tile_destination = targetAnthill->get_steal_hole_tile_position();
					}
					else option = 0;
					
				}
				
				if (order_ant_use_special_ability(apply_orders_now, targetAnt, tile_destination.x, tile_destination.y, option) )
				{
					
					play_sound(s_moving1);
					
					// repassa o input online
					/*
					if (netController.is_online() )
						communicate_ant_ability_use(targetAnt, tile_destination.x, tile_destination.y, option);
					*/
					
				}
				else play_sound(s_no_no);
				
			}
			
		}
		
	}
	
	// atalhos omitidos caso esteja co chat aberto
	if ( !(console.active && console.inputFocus) )
	{
		
		// atalho pra chocar ovo
		if (key_down[KEY_J])
		{
			
			if (AntHill * myHill = get_AntHill(localPlayer) )
			{
				if (myHill->can_hatch_egg() )
				{
					
					order_anthill_hatch_egg(apply_orders_now, *myHill);
					//myHill->hatch_egg();
					
					/*
					if (is_online() )
						communicate_egg_hatch(*myHill);
					*/
					
				}
				else play_sound(s_error);
			}
			
		}
		
		// todas formigas irem pro formigueiro
		if (key_down[KEY_H])
		{
			if (!selectedAnt.empty() )
				input_order_selected_ant_back_to_anthill();
		}
		
		// control groups
		if (key_down[KEY_1])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(1);
			else
				select_control_group(1);
		}
		
		else if (key_down[KEY_2])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(2);
			else
				select_control_group(2);
		}
		
		else if (key_down[KEY_3])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(3);
			else
				select_control_group(3);
		}
		
		else if (key_down[KEY_4])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(4);
			else
				select_control_group(4);
		}
		
		else if (key_down[KEY_5])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(5);
			else
				select_control_group(5);
		}
		
		else if (key_down[KEY_6])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(6);
			else
				select_control_group(6);
		}
		
		else if (key_down[KEY_7])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(7);
			else
				select_control_group(7);
		}
		
		else if (key_down[KEY_8])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(8);
			else
				select_control_group(8);
		}
		
		else if (key_down[KEY_9])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(9);
			else
				select_control_group(9);
		}
		
		else if (key_down[KEY_0])
		{
			if (key_pressed[KEY_LCTRL])
				set_selected_ants_control_group(10);
			else
				select_control_group(10);
		}
		
		
		if (key_down[KEY_A])
		{
			if (key_pressed[KEY_LCTRL])
				select_all_ants();
		}
		
		
		// debug
		#ifdef _DEBUG_MODE 
		if (key_down[KEY_F9])
		{
            if (localPlayer)
                localPlayer->foodPoints += 1000;
		}
		
		if (key_down[KEY_F12])
		{
            show_map_debug = !show_map_debug;
		}
        #endif
	
	}
	
	
}



// local player input
bool GameManager::input_order_selected_ant_back_to_anthill()
{
	
	if (!localPlayer) return false;
	
	bool success = false;
	
	if (AntHill * myHill = get_AntHill(localPlayer) )
	{
		
		const bool apply_orders_now = is_offline();
		IntVector2 pos = myHill->get_hole_tile_position();
		
		success = order_ant_move(apply_orders_now, selectedAnt, pos.x, pos.y, false);
		
		if (success)
			play_sound(s_moving1);
		else 
			play_sound(s_no_no);
		
	}
	
	return success;
	
}


void GameManager::refresh_camera()
{
    
    /*
    const float screen_width = screen_w;
	const float screen_height = screen_h;
	
	camera.target = (Vector2) { screen_width / 2.0f, screen_height / 2.0f };
    camera.offset = (Vector2) { screen_width / 2.0f, screen_height / 2.0f };
    */
}



void GameManager::compute_mouse_camera_movement(float min_x, float min_y, float max_x, float max_y)
{
	
	Vector2 mouse_position(mouse_x, mouse_y);
	
	// dragging no mapa
	if (mouse_middle_down) //&& !mouse_over_any_window() )
	{
		mapDragging = true;
		mapDragPosition.x = mouse_position.x; 
		mapDragPosition.y = mouse_position.y;
	}
	
	// soltando dragging
	if (mouse_middle_released && mapDragging)
		mapDragging = false;
	
	if (mapDragging)
	{
		
		if (mapDragPosition.x != mouse_position.x || mapDragPosition.y != mouse_position.y)
		{
			
			//Camera.move_distance_relative( (mapDragPosition.x - mouse_position.x), (mapDragPosition.y - mouse_position.y) );
			const float speed = 1.0f / camera.zoom;
			camera.x += (mapDragPosition.x - mouse_position.x) * speed;
			camera.y += (mapDragPosition.y - mouse_position.y) * speed;
			
			mapDragPosition.x = mouse_position.x;
			mapDragPosition.y = mouse_position.y;
			//Camera.constrain_position(min_x, min_y, max_x, max_y);
			
		}
		
	}
	
	else if (edgeScrolling)
	{
		
		// scroll
		if (mouse_position.x <= 0 			 && camera.x > min_x) camera.move_left();
		if (mouse_position.x >= screen_w - 1 && camera.x < max_x) camera.move_right();
		if (mouse_position.y <= 0 			 && camera.y > min_y) camera.move_up();
		if (mouse_position.y >= screen_h - 1 && camera.y < max_y) camera.move_down();
		
		//camera.compute_edge_scrolling(min_x, min_y, max_x, max_y);
		
	}
	
}
