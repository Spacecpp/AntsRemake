#include "GameManager.h"
#include "interface_list.h"
#include "Colors.h"


void GameManager::main_menu_window()
{

	const float width = 260.0f;
	const float height = 280.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f * 1.2f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Main Menu", WINDOW_ID::MAIN_MENU, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	const float button_width = 32 * 3; //160.0f;
	const float button_height = 32.0f;
	const float button_offset = 40.0f;
	
	const float button_x = width * 0.5f;
	float button_y = 20.0f;
	
	_GUI_Window.create_button( GUI_Button("Single Player", BUTTON_ID::SINGLE_PLAYER, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	_GUI_Window.create_button( GUI_Button("Online", BUTTON_ID::ONLINE_MENU, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	//_GUI_Window.create_button( GUI_Button("Host Game", BUTTON_ID::HOST_GAME, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	//_GUI_Window.create_button( GUI_Button("Join Game", BUTTON_ID::JOIN_GAME, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	_GUI_Window.create_button( GUI_Button("Options", BUTTON_ID::OPTIONS, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	_GUI_Window.create_button( GUI_Button("Map Editor", BUTTON_ID::MAP_EDITOR, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	_GUI_Window.create_button( GUI_Button("Game Help", BUTTON_ID::HELP_SCREEN, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	_GUI_Window.create_button( GUI_Button("Exit", BUTTON_ID::EXIT, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
	button_y += button_offset;
	
	/*
	const float textbox_x = 80.0f;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(textbox_x, button_y, 200, 30, 0, "Map", ANY_CHARACTER) ) )
	{
		current->link_to_variable(selectedMapName);
		button_y += button_offset;
	}
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(textbox_x, button_y, 200, 30, 0, "Command Delay", NUMBERS_ONLY) ) )
	{
		current->link_to_variable(commandDelay);
		button_y += button_offset;
	}
	*/
	
}



void GameManager::game_setup_window()
{
	
	inGameSetupScreen = true;
	
	const bool online = is_online();
	
	const float screen_width = screen_w;
	const float screen_height = screen_h;
	
	const float width = 500 + 40 + 30 + 20;
	const float height = 640.0f;
	const float x = (screen_width - width) * 0.5f;
	const float y = (screen_height - height) * 0.5f;
	
	const String title = online ? "Multiplayer Game" : "Single Player Game";
	
	_GUI_Window = GUI_Window(x, y, width, height, title, WINDOW_ID::GAME_SETUP, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	
	// caixa dos players
	const float box_x = 20, box_y = 20;
	GUI_Image playerSpace(&window9sliceInsideSprite, WHITE, box_x, box_y, 256, 256);
	playerSpace.set_9slicing(2.0f, 4.0f, 4.0f);
	_GUI_Window.create_image(playerSpace); 
	

	for (Player & currentPlayer : player)
	{
		if (currentPlayer.active)
			create_player_setup_widgets(currentPlayer);
	}
	
	// ----
	
	const float button_width = 32 * 3; //160.0f;
	const float button_height = 32.0f;
	const float button_offset_x = button_width + 10.0f;
	const float button_offset_y = button_height + 10.0f;
	(void)button_offset_y;
	
	float button_x = 20.0f; //width * 0.5f;
	float button_y = height - 306.0f - 16; 
	
	if (GUI_Button * start_button = _GUI_Window.create_button( GUI_Button("Start Game", BUTTON_ID::START_GAME, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_LEFT ) ) 
	{
		if (is_client() )
			start_button->locked = true;
		button_x += button_offset_x;
	}
	
	_GUI_Window.create_button( GUI_Button( online ? "Disconnect" : "Leave", BUTTON_ID::LEAVE_GAME, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_LEFT ); button_x += button_offset_x;
	
	if (online)
	{
		_GUI_Window.create_button( GUI_Button( "Ready", BUTTON_ID::READY, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_LEFT ); button_x += button_offset_x;
	}
	
	else //if (!online)
	{
		_GUI_Window.create_button( GUI_Button( "+Bot", BUTTON_ID::ADD_BOT, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_LEFT ); button_x += button_offset_x;
		_GUI_Window.create_button( GUI_Button( "-Bot", BUTTON_ID::REMOVE_BOT, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_LEFT ); button_x += button_offset_x;
	}
	
	
	
	
	float right_x = width - 40;
	float gui_y = 20;// + 30;
	
	// selecao de mapa
	if (GUI_Button * button = _GUI_Window.create_button( GUI_Button("S", BUTTON_ID::SELECT_MAP_WINDOW, 0, right_x, gui_y, button_height, button_height), WINDOW_ALIGN_RIGHT ) )
	{
		if (is_client() )
			button->locked = true;
	}
	
	const float textbox_width = 200.0f;
	const float textbox_height = 32.0f;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(right_x - textbox_width - button_height, gui_y, textbox_width, textbox_height, 0, "Map", ANY_CHARACTER, WHITE, WHITE) ) )
	{
		
		current->link_to_variable(selectedMapName);
		current->locked = true;
		current->fixed_colors = true;
		current->border_color = WHITE;
		//current->name_on_top;
		gui_y += textbox_height + 6;
		
	}
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(right_x - textbox_width - button_height, gui_y, textbox_width, textbox_height, 0, "Info", ANY_CHARACTER, WHITE, WHITE) ) )
	{
		
		current->link_to_variable(gameMap.description);
		current->locked = true;
		current->fixed_colors = true;
		current->border_color = WHITE;
		//current->name_on_top;
		gui_y += textbox_height + 6;
		
	}
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(right_x - textbox_width - button_height, gui_y, textbox_width, textbox_height, 0, "Time", ANY_CHARACTER, WHITE, WHITE) ) )
	{
		
		//current->link_to_variable(gameMap.description);
		
		int seconds = gameMap.gameTime;
		int minutes = seconds / 60;
		seconds = seconds - (minutes * 60);
		
		String str;
		
		if (minutes > 0)
		{
			str += to_stringi(minutes) + " minutes";
			if (seconds > 0)
				str += " and ";
		}
		
		if (seconds > 0)
			str += to_stringi(seconds) + " seconds";
			
		
		current->set_text(str);
		current->locked = true;
		current->fixed_colors = true;
		current->border_color = WHITE;
		//current->name_on_top;
		gui_y += 80;
		
	}
	// ----
	
	if (online)
	{
		const float slider_width = (60 - 2) * 2;
		const float x = right_x - slider_width;
		const float y = button_y; //gui_y;
		
		if (GUI_Slider * slider = _GUI_Window.create_slider( GUI_Slider(x, y, slider_width, 16, "Command delay", 2, 60, true, commandDelay) ) )
		{
			if (is_client() )
				slider->locked = true;
		}
	}
	
	
	/*
	std::vector<ColorRGBA> colors;
	std::vector<size_t> indexes;
	
	for (int i = 0; i < TOTAL_COLORS; i++)
	{
		colors.push_back(get_indexed_color(i) );
		indexes.push_back(i);
	}
	
	
	GUI_ColorChooser cc(20, 20, NULL, 64, 64);
	cc.set_indexed_colors(colors, indexes, 0, TOTAL_COLORS);
	cc.link_to_variable( &player[0].color);
	cc.link_to_variable( &player[0].color_index);
	
	_GUI_Window.create_color_chooser(cc);
	*/
	
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
	
	place_console();
	
	if (!gameMap.loaded || selectedMapName != gameMap.name)
	{
		
		if (!gameMap.loaded)
			printf("!gameMap.loaded\n");
		
		if (selectedMapName != gameMap.name)
			printf("%s - %s\n", selectedMapName.c_str(), gameMap.name.c_str() );
		
		printf("Loading map on setup window creation\n");
		load_selected_map();
	}
	
}



void GameManager::create_player_setup_widgets(Player & targetPlayer)
{
	
	if (!_GUI_Window.active || _GUI_Window.code != WINDOW_ID::GAME_SETUP) return;
	
	// color choosers
	static std::vector<ColorRGBA> colors;
	static std::vector<size_t> indexes;
	
	if (colors.empty() || indexes.empty() ) // 1-time setup
	{
		colors.reserve(TOTAL_COLORS);
		indexes.reserve(TOTAL_COLORS);
		
		for (int i = 0; i < TOTAL_COLORS; i++)
		{
			colors.push_back(get_indexed_bright_color(i) );
			indexes.push_back(i);
		}
	}
	
	
	float box_y = 20;
	float offset = 64.0f;
	const float player_widget_y = box_y + (float)targetPlayer.index * offset;
	
	
	const float color_chooser_x = 20;
	const float color_chooser_width = 64;
	const float name_label_x = color_chooser_x + color_chooser_width + 10.0f;
	
	GUI_ColorChooser cc(color_chooser_x, player_widget_y, NULL, color_chooser_width, color_chooser_width);
	cc.set_color_button_size(32, 32);
	cc.set_window_offset(1, 0);
	cc.set_indexed_colors(colors, indexes, 4, 2);
	cc.invisibleBaseButton = true;
	//cc.link_to_variable(currentPlayer.color);
	cc.link_to_variable(targetPlayer.colorIndex);
	cc.instantOpenClose = true;
	cc.locked = targetPlayer.is_remote();
	
	_GUI_Window.create_color_chooser(cc);
	
	
	//targetPlayer.get_color()
	_GUI_Window.create_label(GUI_Label(name_label_x, player_widget_y + 20.0f, targetPlayer.name, the_font, WHITE, TEXT_ALIGN_LEFT, 32) );
	
	
}


void GameManager::remove_player_setup_widgets(Player & targetPlayer)
{
	
	if (!_GUI_Window.active || _GUI_Window.code != WINDOW_ID::GAME_SETUP) return;
	
	
	// TODO: trocar pra sistema de id de widget, converter pra reverse iterator
	for (int a = _GUI_Window.colorChooser.size() - 1; a >= 0; a--)
	{
		
		GUI_ColorChooser & current = _GUI_Window.colorChooser[a];
		if (current.active)
		{
			if (current.linked_index == &targetPlayer.colorIndex)
			{
				_GUI_Window.destroy_child(current);
				break;
			}
		}
		
	}
	
	
	for (int a = _GUI_Window.label.size() - 1; a >= 0; a--)
	{
		
		GUI_Label & current = _GUI_Window.label[a];
		if (current.active)
		{
			if (current.text == targetPlayer.name)
			{
				_GUI_Window.destroy_child(current);
				break;
			}
		}
		
	}
	
	
}



void GameManager::info_window(const std::string & text, bool back_button)
{
	
	//if (ChatField.active)
	//	close_chat_box();
	
	const float fontSize = GUI_WINDOW_TITLE_FONT_HEIGHT;
	const float width = std::max( the_font.get_text_width_for_height(text, fontSize) + 20.0f, 100.0f);
	const float height = 80.0f;
	const float half_width = width * 0.5f;
	
	//window_frame_sprite
	_GUI_Window = GUI_Window( screen_w * 0.5f - half_width, screen_h * 0.5f - 30, width, height, text.c_str(), WINDOW_ID::INFO, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	//_GUI_Window.auto_scaling = true;
	//_GUI_Window.set_9slicing(10.0f, 0.5f);
	
	const float button_aspect_ratio = 0.421875f;
	const float button_width = 80.0f;
	const float button_height = round_up(button_width * button_aspect_ratio);
	const float button_y = 20.0f; //height - button_height - 20.0f;
	_GUI_Window.create_button( GUI_Button("Ok", back_button ? BUTTON_ID::BACK : BUTTON_ID::OK, 0, half_width, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
	printf("%.2f %.2f %.2f\n", height, _GUI_Window.height, button_y);
	
	//Window.scale(get_display_scale_factor_v() );
	//refresh_selected_window(Window);
	
}


void GameManager::server_not_responding_window()
{
	
	String text = "Server not responding";
	
	const float fontSize = 30.0f;
	const float width = std::max( the_font.get_text_width_for_height(text, fontSize) + 20.0f + 10.0f, 100.0f);
	const float height = 80.0f;
	const float half_width = width * 0.5f;
	
	_GUI_Window = GUI_Window( screen_w * 0.5f - half_width, screen_h * 0.5f - 30, width, height, text.c_str(), WINDOW_ID::SERVER_NOT_RESPONDING, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	const float button_aspect_ratio = 0.421875f;
	const float button_width = 80.0f;
	const float button_height = round_up(button_width * button_aspect_ratio);
	const float button_y = 20.0f; 
	
	_GUI_Window.create_button( GUI_Button("Retry", BUTTON_ID::JOIN_GAME, 0, width * 0.25f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	_GUI_Window.create_button( GUI_Button("Cancel", BUTTON_ID::BACK, 0, width * 0.75f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}


void GameManager::esc_menu_window()
{
	
	//const float fontSize = 30.0f;
	const float width = 300.0f;
	const float height = 90.0f;
	const float half_width = width * 0.5f;
	String title = editMode ? "Really leave map editor?" : "Really leave game?";
	
	_GUI_Window = GUI_Window( screen_w * 0.5f - half_width, screen_h * 0.5f - 30, width, height, title, WINDOW_ID::INGAME_ESC, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	const float button_aspect_ratio = 0.421875f;
	const float button_width = 80.0f;
	const float button_height = round_up(button_width * button_aspect_ratio);
	const float button_y = 20.0f; 
	
	_GUI_Window.create_button( GUI_Button("Leave", BUTTON_ID::LEAVE_GAME, 0, width * 0.25f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	_GUI_Window.create_button( GUI_Button("Cancel", BUTTON_ID::OK, 0, width * 0.75f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}


void GameManager::online_window()
{
	
	const float width = 300.0f;
	const float height = 420.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Online", WINDOW_ID::ONLINE, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	const float button_width = 32 * 3; //160.0f;
	const float button_height = 32.0f;
	const float button_offset = 40.0f;
	
	//const float button_x = width * 0.5f;
	float button_y = 20.0f;
	

	_GUI_Window.create_button( GUI_Button("Host Game", BUTTON_ID::HOST_GAME, 0, width * 0.25f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); 
	_GUI_Window.create_button( GUI_Button("Join Game", BUTTON_ID::JOIN_GAME, 0, width * 0.75f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); 
	button_y += button_offset;
	_GUI_Window.create_button( GUI_Button("Back", BUTTON_ID::BACK, 0, width * 0.5f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER ); button_y += button_offset;
	
	const float textbox_x = 80.0f;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(textbox_x, button_y, 200, 30, 0, "Nickname", ANY_CHARACTER) ) )
	{
		current->link_to_variable(localPlayerName);
		button_y += button_offset;
	}
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(textbox_x, button_y, 200, 30, 0, "Join IP", ANY_CHARACTER) ) )
	{
		current->link_to_variable(joinIP);
		button_y += button_offset;
	}
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(textbox_x, button_y, 200, 30, 0, "Port", ANY_CHARACTER) ) )
	{
		current->link_to_variable(port);
		button_y += button_offset;
	}
	
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
	for (GUI_TextBox & current : _GUI_Window.textBox)
	{
		current.text_color = WHITE;
		current.background_color = WHITE;
	}
	
	
}


void GameManager::select_map_window()
{
	
	console.destroy();
	
	const float width = 300.0f;
	const float height = 420.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Select Map", WINDOW_ID::SELECT_MAP, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	
	if (GUI_ListBox * listbox = _GUI_Window.create_listbox( GUI_ListBox(20, 20, width - 40, height - 80, "Maps/", ".map") ) )
	{
		
		listbox->core_color = WHITE;
		listbox->border_color = WHITE;
		
		GUI_listbox_text_color = WHITE;
		GUI_listbox_highlighted_text_color = BLACK;
		GUI_listbox_hightlight_color = WHITE;
		
	}
	
	const float button_y = height - 50.0f;
	const float button_width = 32 * 3;
	const float button_height = 32.0f;
	
	_GUI_Window.create_button( GUI_Button("Select", BUTTON_ID::SELECT_MAP, 0, width * 0.25f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	_GUI_Window.create_button( GUI_Button("Cancel", BUTTON_ID::BACK, 0, width * 0.75f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}


void GameManager::options_window()
{
	
	const float width = 300.0f;
	const float height = 420.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Options", WINDOW_ID::OPTIONS, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	const float button_width = 32 * 3; //160.0f;
	const float button_height = 32.0f;
	//const float button_offset = 40.0f;
	
	
	_GUI_Window.create_checkbox( GUI_CheckBox("Edge Scrolling", edgeScrolling, 20, 10) );
	
	float button_y = height - button_height - 10;
	_GUI_Window.create_button( GUI_Button("Ok", BUTTON_ID::BACK, 0, width * 0.5f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}


void GameManager::end_game_window()
{
	
	const float width = 540.0f;
	const float height = 320.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Final Score", 0, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	const float button_y = height - 50.0f;
	
	// painel dentro da janela
	const float inner_offset = 10.0f;
	const float box_width = width - inner_offset * 2;
	const float box_height = button_y - inner_offset * 2;
	GUI_Image black_box(&window9sliceInsideSprite, WHITE, inner_offset, inner_offset, box_width, box_height);
	black_box.set_9slicing(2.0f, 4, 4);
	_GUI_Window.create_image(black_box);
	
	
	// cria lista ordenada
	std::vector<Player*> sortedPlayers;
	for (Player & currentPlayer : player)
	{
		if (currentPlayer.active)
			sortedPlayers.push_back(&currentPlayer);
	}
	std::sort(sortedPlayers.begin(), sortedPlayers.end(), sortPlayerScore);
	
	
	// cria labels
	const float font_height = 32.0f;
	const float text_scale = the_font.get_scale_for_height(font_height);
	
	float gui_y = 10.0f + inner_offset;
	const float line_offset = font_height + 10.0f;
	
	const float name_x = 10.0f + inner_offset;
	const float hatches_x = width - 10.0f - inner_offset;
	const float deaths_x = hatches_x - 100.0f;
	const float kills_x = deaths_x - 90.0f;
	const float score_x = kills_x - 80.0f;
	
	_GUI_Window.create_label( GUI_Label(name_x, gui_y, "Name", the_font, WHITE, TEXT_ALIGN_LEFT, text_scale) );
	_GUI_Window.create_label( GUI_Label(score_x, gui_y, "Score", the_font, WHITE, TEXT_ALIGN_RIGHT, text_scale) );
	_GUI_Window.create_label( GUI_Label(kills_x, gui_y, "Kills", the_font, WHITE, TEXT_ALIGN_RIGHT, text_scale) );
	_GUI_Window.create_label( GUI_Label(deaths_x, gui_y, "Deaths", the_font, WHITE, TEXT_ALIGN_RIGHT, text_scale) );
	_GUI_Window.create_label( GUI_Label(hatches_x, gui_y, "Hatches", the_font, WHITE, TEXT_ALIGN_RIGHT, text_scale) );
	gui_y += line_offset;
	
	for (Player * currentPlayerP : sortedPlayers)
	{
		
		Player & currentPlayer = *currentPlayerP;
		
		ColorRGBA color = currentPlayer.get_color();
		
		_GUI_Window.create_label( GUI_Label(name_x,    gui_y, currentPlayer.name, 					the_font, color, TEXT_ALIGN_LEFT,  text_scale) );
		_GUI_Window.create_label( GUI_Label(score_x,   gui_y, to_stringi(currentPlayer.foodPoints), the_font, color, TEXT_ALIGN_RIGHT, text_scale) );
		_GUI_Window.create_label( GUI_Label(kills_x,   gui_y, to_stringi(currentPlayer.kills),	    the_font, color, TEXT_ALIGN_RIGHT, text_scale) );
		_GUI_Window.create_label( GUI_Label(deaths_x,  gui_y, to_stringi(currentPlayer.deaths),	    the_font, color, TEXT_ALIGN_RIGHT, text_scale) );
		_GUI_Window.create_label( GUI_Label(hatches_x, gui_y, to_stringi(currentPlayer.hatches),    the_font, color, TEXT_ALIGN_RIGHT, text_scale) );
		gui_y += line_offset;
		
	}
	
	
	
	const float button_width = 32 * 3;
	const float button_height = 32.0f;
	
	_GUI_Window.create_button( GUI_Button("Leave Game", BUTTON_ID::LEAVE_GAME, 0, width * 0.25f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	_GUI_Window.create_button( GUI_Button("Observe", BUTTON_ID::OK, 0, width * 0.75f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}


void GameManager::create_map_editor_upper_menu_window()
{
	
	const float button_width = 32.0f;
	const float small_distance = 10.0f;
	
	const float width = button_width * EDITOR_ICON_SPRITE_COUNT + small_distance * 2; //380.0f;
	const float height = button_width;
	
	mapEditor_upperMenu = GUI_Window(0, 0, width, height, "", 0, false);
	mapEditor_upperMenu.set_alignment(WINDOW_ALIGN_LEFT, WINDOW_ALIGN_TOP);
	mapEditor_upperMenu.body_slice_border = 4.0f;
	mapEditor_upperMenu.body_slice_scale = 2.0f;
	mapEditor_upperMenu.body_color = WHITE; //ColorRGBA(0.1f, 0.1f, 0.1f, 0.4f);
	mapEditor_upperMenu.titlebar_height = 0;
	
	
	const float offset = button_width;
	float x = 0, y = 0;
	
	/*
	mapEditor_upperMenu.create_button( GUI_Button("N", BUTTON_ID::EDITOR_NEW_MAP, 0, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("O", BUTTON_ID::SELECT_MAP_WINDOW, 0, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("S", BUTTON_ID::EDITOR_SAVE_MAP_WINDOW, 0, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	x += 10;
	mapEditor_upperMenu.create_button( GUI_Button("P", BUTTON_ID::EDITOR_MAP_PROPERTIES_WINDOW, 0, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("R", BUTTON_ID::EDITOR_RESIZE_MAP_WINDOW, 0, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	x += 10;
	mapEditor_upperMenu.create_button( GUI_Button("1", BUTTON_ID::EDITOR_CHANGE_MODE, 0, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("2", BUTTON_ID::EDITOR_CHANGE_MODE, 1, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("3", BUTTON_ID::EDITOR_CHANGE_MODE, 2, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("4", BUTTON_ID::EDITOR_CHANGE_MODE, 3, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("5", BUTTON_ID::EDITOR_CHANGE_MODE, 4, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("6", BUTTON_ID::EDITOR_CHANGE_MODE, 5, x, y, button_width, button_width), WINDOW_ALIGN_LEFT ); x += offset;
	*/
	
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_NEW_MAP, 0, x, y, button_width, button_width, &editorIconSprite[0]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::SELECT_MAP_WINDOW, 0, x, y, button_width, button_width, &editorIconSprite[1]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_SAVE_MAP_WINDOW, 0, x, y, button_width, button_width, &editorIconSprite[2]), WINDOW_ALIGN_LEFT ); x += offset;
	x += small_distance;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_MAP_PROPERTIES_WINDOW, 0, x, y, button_width, button_width, &editorIconSprite[3]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_RESIZE_MAP_WINDOW, 0, x, y, button_width, button_width, &editorIconSprite[4]), WINDOW_ALIGN_LEFT ); x += offset;
	x += small_distance;
	
	for (int i = 5; i < EDITOR_ICON_SPRITE_COUNT; i++)
	{
		mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, i - 5, x, y, button_width, button_width, &editorIconSprite[i]), WINDOW_ALIGN_LEFT ); 
		x += offset;
		
	}
	
	/*
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 0, x, y, button_width, button_width, &editorIconSprite[5]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 1, x, y, button_width, button_width, &editorIconSprite[6]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 2, x, y, button_width, button_width, &editorIconSprite[7]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 3, x, y, button_width, button_width, &editorIconSprite[8]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 4, x, y, button_width, button_width, &editorIconSprite[9]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 5, x, y, button_width, button_width, &editorIconSprite[10]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 6, x, y, button_width, button_width, &editorIconSprite[11]), WINDOW_ALIGN_LEFT ); x += offset;
	mapEditor_upperMenu.create_button( GUI_Button("", BUTTON_ID::EDITOR_CHANGE_MODE, 7, x, y, button_width, button_width, &editorIconSprite[12]), WINDOW_ALIGN_LEFT ); x += offset;*/
	
}


void GameManager::create_map_editor_tool_window()
{
	
	bool reached_last_page = false;
	
	const float button_width = 64.0f;
	const float small_button_width = 32.0f;
	const float offset = 4.0f;
	const int rows = 5;
	const int colluns = 3;
	
	const float width = colluns * button_width + (colluns + 1) * offset;
	const float height = rows * button_width + (rows + 1) * offset + small_button_width + offset * 2;
	
	float window_x, window_y;
	if (mapEditor_toolWindow.active)
	{
		window_x = mapEditor_toolWindow.x;
		window_y = mapEditor_toolWindow.y;
	}
	else
	{
		window_x = screen_w - width;
		window_y = screen_h - height;
	}
	
	mapEditor_toolWindow = GUI_Window(window_x, window_y, width, height, "Brush", 0, false);
	mapEditor_toolWindow.set_alignment(WINDOW_ALIGN_RIGHT, WINDOW_ALIGN_BOTTOM);
	mapEditor_toolWindow.body_slice_border = 4.0f;
	mapEditor_toolWindow.body_slice_scale = 2.0f;
	mapEditor_toolWindow.body_color = WHITE;
	mapEditor_toolWindow.titlebar_height = -32;
	mapEditor_toolWindow.draggable = true;
	
	
	float x = offset, y = offset;
	const bool use_color_buttons = (editorElementType == EDITOR_ELEMENT::ANTHILL || editorElementType == EDITOR_ELEMENT::ANT || editorElementType == EDITOR_ELEMENT::TERRAIN_OBJECT);
	if (use_color_buttons)
	{
		
		const float color_button_width = (width - (offset * (MAX_PLAYERS + 1) ) ) / MAX_PLAYERS;
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			
			mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_PLAYER_ID, i, x, y, color_button_width, color_button_width, &neutralFrameSprite, get_indexed_bright_color(i) ), WINDOW_ALIGN_LEFT );
			
			x += color_button_width + offset;
			
		}
		
		x = offset;
		y += color_button_width + offset;
		
	}
	
	
	int collum_count = 0;
	
	switch (editorElementType)
	{
		
		case EDITOR_ELEMENT::TERRAIN:
		{
			for (int i = 0; i < TILE::COUNT; i++)
			{
				
				Bitmap * bitmap = NULL;
				if (i == TILE::DIRT)
					bitmap = &dirtTexture;
					
				else if (i == TILE::SAND)
					bitmap = &sandTexture;
				
				else if (i == TILE::ROCKY_GROUND)
					bitmap = &rockyGroundTexture;
				
				else if (i == TILE::MUD)
					bitmap = &mudTexture;
					
				else if (i == TILE::WATER)
					bitmap = &waterTexture;
				
				else if (i == TILE::_VOID_)
					bitmap = &voidTileTexture;
					
				
				mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, i, x, y, button_width, button_width, bitmap), WINDOW_ALIGN_LEFT ); 
				
				
				collum_count++;
				if (collum_count < colluns)
					x += offset + button_width;
				else
				{
					x = offset;
					y += offset + button_width;
					collum_count = 0;
				}
				
			}
			break;
			
		}
		
		case EDITOR_ELEMENT::TERRAIN_OBJECT:
		{
			
			//namespace SECONDARY_TILE { enum { NONE, FIRE, MUD_BRIDGE, LANDMINE_0 }; } 
			
			mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, SECONDARY_TILE::FIRE, x, y, button_width, button_width, &fireSprite), WINDOW_ALIGN_LEFT ); x += offset + button_width;
			
			mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, SECONDARY_TILE::MUD_BRIDGE, x, y, button_width, button_width, &mudBridgeSprite), WINDOW_ALIGN_LEFT ); x += offset + button_width;
			
			mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, SECONDARY_TILE::LANDMINE_0, x, y, button_width, button_width, &landMineSprite), WINDOW_ALIGN_LEFT ); x += offset + button_width;
			
			break;
			
		}
		
		case EDITOR_ELEMENT::ANTHILL:
		{
			
			const float cbox_width = 16.0f;
			const float offset = cbox_width + 4;
			
			if (GUI_CheckBox * current = mapEditor_toolWindow.create_checkbox( GUI_CheckBox(false, "Flip horizontal", x, y, cbox_width) ) )
			{
				current->link_to_variable_bit(editorAntHillFlags, 0);
				current->locked = true;
				y += offset;
			}
			
			if (GUI_CheckBox * current = mapEditor_toolWindow.create_checkbox( GUI_CheckBox(false, "Flip vertical", x, y, cbox_width) ) )
			{
				current->link_to_variable_bit(editorAntHillFlags, 1);
				current->locked = true;
				y += offset;
			}
			
			if (GUI_CheckBox * current = mapEditor_toolWindow.create_checkbox( GUI_CheckBox(false, "Symetrical", x, y, cbox_width) ) )
			{
				current->link_to_variable_bit(editorAntHillFlags, 2);
				y += offset;
			}
			
			y += cbox_width;
			
			const float w = width - offset * 2.0f;
			mapEditor_toolWindow.create_image( GUI_Image(&antHillDefaultSprite[0], WHITE, x, y, w, w) );
			
			break;
			
		}
		
		case EDITOR_ELEMENT::ANT:
		{
			
			for (int i = 0; i < POWERUP::COUNT; i++)
			{
				
				Bitmap * bitmap = &antFirstFrameSprite[i];
				mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, i, x, y, button_width, button_width, bitmap), WINDOW_ALIGN_LEFT ); 
				
				
				collum_count++;
				if (collum_count < colluns)
					x += offset + button_width;
				else
				{
					x = offset;
					y += offset + button_width;
					collum_count = 0;
				}
				
			}
			break;
			
		}
		
		case EDITOR_ELEMENT::POWERUP:
		{
			
			for (int i = 1; i < POWERUP::COUNT; i++)
			{
				
				Bitmap & bitmap = powerupSprite[i - 1];
				mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, i, x, y, button_width, button_width, &bitmap), WINDOW_ALIGN_LEFT ); 
				
				
				collum_count++;
				if (collum_count < colluns)
					x += offset + button_width;
				else
				{
					x = offset;
					y += offset + button_width;
					collum_count = 0;
				}
				
			}
			break;
			
		}
		
		case EDITOR_ELEMENT::FOOD:
		{
			
			int max_items_per_page = rows * colluns;
			int first_item = editorPage * max_items_per_page;
			int item_count = foodData.size();
			
			// safety
			if (first_item >= item_count)
			{
				first_item = 0;
				editorPage = 0;
			}
			
			
			int it_count = 0;
			for (int i = first_item; ; i++)
			{
				
				FoodData & currentData = foodData[i];
				Bitmap * bitmap = currentData.get_first_frame_sprite();
				mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, i, x, y, button_width, button_width, bitmap), WINDOW_ALIGN_LEFT ); 
				
				
				collum_count++;
				if (collum_count < colluns)
					x += offset + button_width;
				else
				{
					x = offset;
					y += offset + button_width;
					collum_count = 0;
				}
				
				
				// controle de escape
				if (i + 1 >= item_count)
				{
					reached_last_page = true;
					//printf("last page (%d %d)\n", i + 1, item_count);
					break;
				}
				
				it_count++;
				if (it_count >= max_items_per_page)
				{
					//printf("last item (%d %d)\n", it_count, max_items_per_page);
					break;
				}
				
			}
			break;
			
		}
		
		case EDITOR_ELEMENT::OBSTACLE:
		{
			
			int max_items_per_page = rows * colluns;
			int first_item = editorPage * max_items_per_page;
			int item_count = obstacleData.size();
			
			// safety
			if (first_item >= item_count)
			{
				first_item = 0;
				editorPage = 0;
			}
			
			
			int it_count = 0;
			for (int i = first_item; ; i++)
			{
				
				ObstacleData & currentData = obstacleData[i];
				Bitmap * bitmap = currentData.get_first_frame_sprite();
				mapEditor_toolWindow.create_button( GUI_Button("", BUTTON_ID::EDITOR_TYPE_ID, i, x, y, button_width, button_width, bitmap), WINDOW_ALIGN_LEFT ); 
				
				
				collum_count++;
				if (collum_count < colluns)
					x += offset + button_width;
				else
				{
					x = offset;
					y += offset + button_width;
					collum_count = 0;
				}
				
				
				// controle de escape
				if (i + 1 >= item_count)
				{
					reached_last_page = true;
					//printf("last page (%d %d)\n", i + 1, item_count);
					break;
				}
				
				it_count++;
				if (it_count >= max_items_per_page)
				{
					//printf("last item (%d %d)\n", it_count, max_items_per_page);
					break;
				}
				
			}
			break;
			
		}
		
		case EDITOR_ELEMENT::POWERUP_GENERATOR:
		{
			
			
			if (GUI_TextBox * current = mapEditor_toolWindow.create_text_box( GUI_TextBox(10, 40, width - 20, -1, 0, "Spawn time (sec)", POSITIVE_INTEGERS_ONLY, WHITE, WHITE) ) )
			{
				
				current->link_to_variable(editorPowerupGenTime);
				current->name_on_top = true;
				current->border_color = WHITE;
				
			}
			
			break;
			
		}
		
	}
	
	y = height - small_button_width - offset;
	
	// page buttons < >
	const bool include_page_buttons = (editorElementType == EDITOR_ELEMENT::FOOD || editorElementType == EDITOR_ELEMENT::OBSTACLE);
	if (include_page_buttons)
	{
		
		GUI_Button prev_button("<", BUTTON_ID::EDITOR_PREV_PAGE, 0, width * 0.5f - offset, y, small_button_width, small_button_width);
		if (editorPage <= 0)
			prev_button.locked = true;
		
		GUI_Button next_button(">", BUTTON_ID::EDITOR_NEXT_PAGE, 0, width * 0.5f + offset, y, small_button_width, small_button_width);
		if (reached_last_page)
			next_button.locked = true;
		
		mapEditor_toolWindow.create_button( prev_button, WINDOW_ALIGN_RIGHT ); 
		mapEditor_toolWindow.create_button( next_button, WINDOW_ALIGN_LEFT ); 
		
	}


}


void GameManager::editor_save_map_window()
{
	
	const float width = 300.0f;
	const float height = 100.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Save Map", WINDOW_ID::SAVE_MAP, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	/*
	if (GUI_ListBox * listbox = _GUI_Window.create_listbox( GUI_ListBox(20, 20, width - 40, height - 80, "Maps/", ".map") ) )
	{
		
		listbox->core_color = WHITE;
		listbox->border_color = WHITE;
		
		GUI_listbox_text_color = WHITE;
		GUI_listbox_highlighted_text_color = BLACK;
		GUI_listbox_hightlight_color = WHITE;
		
	}*/
	
	const float textbox_width = 200.0f;
	const float textbox_height = 32.0f;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(60, height - 90, textbox_width, textbox_height, 0, "Name", ANY_CHARACTER, WHITE, WHITE) ) )
	{
		
		current->link_to_variable(editorSaveFilename);
		current->border_color = WHITE;
		
	}
	
	
	const float button_y = height - 50.0f;
	const float button_width = 32 * 3;
	const float button_height = 32.0f;
	
	_GUI_Window.create_button( GUI_Button("Save", BUTTON_ID::EDITOR_SAVE_MAP, 0, width * 0.25f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	_GUI_Window.create_button( GUI_Button("Cancel", BUTTON_ID::BACK, 0, width * 0.75f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}



void GameManager::editor_map_properties_window()
{
	
	const float width = 360.0f;
	const float height = 260.0f;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window = GUI_Window(x, y, width, height, "Map Properties", 0, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32.0f;
	
	
	float gui_x = 110.0f;
	float gui_y = 10.0f;
	
	const float textbox_width = 200.0f;
	const float textbox_height = 32.0f;
	
	const float offset = textbox_height + 6.0f;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(gui_x, gui_y, textbox_width, textbox_height, 0, "Name", ANY_CHARACTER, WHITE, WHITE) ) )
	{
		current->link_to_variable(gameMap.name);
		current->allow_long_text = true;
		current->border_color = WHITE;
	}
	gui_y += offset;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(gui_x, gui_y, textbox_width, textbox_height, 0, "Time (secs)", NUMBERS_ONLY, WHITE, WHITE) ) )
	{
		current->link_to_variable(gameMap.gameTime);
		current->border_color = WHITE;
	}
	gui_y += offset;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(gui_x, gui_y, textbox_width, textbox_height, 0, "Eggs", NUMBERS_ONLY, WHITE, WHITE) ) )
	{
		current->link_to_variable(gameMap.eggs);
		current->border_color = WHITE;
	}
	gui_y += offset;
	
	if (GUI_TextBox * current = _GUI_Window.create_text_box( GUI_TextBox(gui_x, gui_y, textbox_width, textbox_height, 0, "Description", ANY_CHARACTER, WHITE, WHITE) ) )
	{
		current->link_to_variable(gameMap.description);
		current->allow_long_text = true;
		current->border_color = WHITE;
	}
	gui_y += offset;
	
	
	std::vector<string_int> items;
	items.reserve(POWERUP::COUNT);
	for (int i = 0; i < POWERUP::COUNT; i++)
		items.push_back( string_int(get_powerup_name(i), i) );
	
	if (GUI_DropdownBox * current = _GUI_Window.create_dropdown_box( GUI_DropdownBox(gui_x, gui_y, 200, 32, "Default Powerup", items, false) ) )
	{
		current->link_to_variable(gameMap.defaultPowerup);
	}
	
	// botoes
	const float button_y = height - 50.0f;
	const float button_width = 32 * 3;
	const float button_height = 32.0f;
	
	_GUI_Window.create_button( GUI_Button("Ok", BUTTON_ID::OK, 0, width * 0.5f, button_y, button_width, button_height), WINDOW_ALIGN_CENTER );
	
	for (GUI_Button & current : _GUI_Window.button)
	{
		current.name_offset_y = -4;
	}
	
}



void GameManager::editor_map_resize_window()
{
	// TODO
	//const bool use_transition = window_switch_routine(WINDOW::RESIZE_MAP);
	//if (FileListBox.active) FileListBox.destroy();

	const float width = 320;
	const float height = 320;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	const std::string title = "Resize map"; //get_translated_string("Resize map");
	
	_GUI_Window.init(x, y, width, height, title, WINDOW_ID::EDITOR_RESIZE_MAP, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32;
	
	const float textbox_width = 40.0f;
	_GUI_Window.create_text_box( GUI_TextBox(110,  30, textbox_width, -1, 0, "Up", NUMBERS_ONLY) );
	_GUI_Window.create_text_box( GUI_TextBox(110, 130, textbox_width, -1, 0, "Down", NUMBERS_ONLY) );
	_GUI_Window.create_text_box( GUI_TextBox(50,   80, textbox_width, -1, 0, "Left", NUMBERS_ONLY) );
	_GUI_Window.create_text_box( GUI_TextBox(180,  80, textbox_width, -1, 0, "Right", NUMBERS_ONLY) );
	
	for (GUI_TextBox & current : _GUI_Window.textBox)
	{
		
		current.name_on_top = true;
		current.text_color = WHITE;
		current.background_color = WHITE;
		
	}
	
	const float button_y = height - 45.0f;
	_GUI_Window.create_button( GUI_Button( "Resize", BUTTON_ID::EDITOR_APPLY_MAP_RESIZE, 0, 20, button_y, 120, -1) );
	_GUI_Window.create_button( GUI_Button( "Cancel", BUTTON_ID::BACK, 0, width - 20, button_y, 120, -1), WINDOW_ALIGN_RIGHT );
	
	//Window.scale(get_interface_scale_factor() );
	//refresh_selected_window(Window);
	
}



void GameManager::confirm_new_map_window()
{
	
	const float width = 300;
	const float height = 80;
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	const std::string title = "Really start a new map?"; 
	
	_GUI_Window.init(x, y, width, height, title, WINDOW_ID::EDITOR_NEW_MAP, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = -32;
	
	
	const float button_y = height - 45.0f;
	_GUI_Window.create_button( GUI_Button( "Yes", BUTTON_ID::EDITOR_NEW_MAP, 0, 20, button_y, 120, -1) );
	_GUI_Window.create_button( GUI_Button( "Cancel", BUTTON_ID::BACK, 0, width - 20, button_y, 120, -1), WINDOW_ALIGN_RIGHT );
	
}


void GameManager::game_help_window()
{
	
	Bitmap & sprite = helpScreenImage;
	const float offset = 6.0f;
	
	const float width = offset * 2 + sprite.get_width();
	const float height = offset * 2 + sprite.get_height();
	const float x = (screen_w - width) * 0.5f;
	const float y = (screen_h - height) * 0.5f;
	
	_GUI_Window.init(x, y, width, height, "", 0, false);
	_GUI_Window.set_alignment(WINDOW_ALIGN_CENTER, WINDOW_ALIGN_CENTER);
	_GUI_Window.body_slice_border = 4.0f;
	_GUI_Window.body_slice_scale = 2.0f;
	_GUI_Window.body_color = WHITE;
	_GUI_Window.titlebar_height = 0;
	
	
	_GUI_Window.create_image( GUI_Image(&sprite, WHITE, offset, offset) );
	
	
	const float button_width = 80.0f;
	const float button_height = 40.0f;
	const float button_x = width - offset - button_width - 20;
	const float button_y = height - offset - button_height - 20;
	
	_GUI_Window.create_button( GUI_Button( "Back", BUTTON_ID::BACK, 0, button_x, button_y, button_width, button_height), WINDOW_ALIGN_LEFT);
	
}
