// alegrinho version
#include "Console.h"

//#include "globals.h" // font
//#define CONSOLE_FONT theFont

extern RenderCache spriteCache;
extern Shader spriteShader;

#ifndef CONSOLE_SCROLL_GLYPH
// must supply these somewhere
extern Bitmap arrowUpSprite;
extern Bitmap arrowDownSprite;
extern Bitmap scrollBoxSprite;
extern Bitmap whiteSquareBitmap;
#endif



ChatString::ChatString(): user_color(1.0f, 1.0f, 1.0f, 1.0f), text_color(1.0f, 1.0f, 1.0f, 1.0f)
{
}


ChatString::ChatString(const char * text, const char * user_name, const ColorRGBA & user_color, const ColorRGBA & text_color):
	text(text), user_name(user_name), user_color(user_color), text_color(text_color)
{
}


ChatString::ChatString(const std::string & text, const std::string & user_name, const ColorRGBA & user_color, const ColorRGBA & text_color):
	text(text), user_name(user_name), user_color(user_color), text_color(text_color)
{
}


/*
int ChatString::get_text_width(ALLEGRO_FONT * font)
{
	
	string combined_string;
	if (!user_name.empty() )
		combined_string = user_name + ": ";
	combined_string += text;
	
	return al_get_text_width(font, combined_string.c_str() );
	
}*/




Console::Console(): active(false), fontSize(CONSOLE::DEFAULT_LINE_HEIGHT), show_input_vertical_line(false), input_vertical_line_time(0),
	sprite(NULL), _9slice_scale(1.0f), _9slice_width(0.0f), font(NULL), shadow_text(true), borders_outside(false)
{
	//text_texture = NULL;
	//mutex = al_create_mutex(); if (!mutex) { show_native_error_message("Failed to create Console mutex!"); }
	reset();
}


Console::~Console()
{
	
	if (text_texture.is_ready() )
		text_texture.destroy();
	//al_destroy_mutex(mutex); mutex = NULL;
}



void Console::set(float x, float y, float width, float height, bool always_opaque, float scale, int fontSize)
{
	
	const bool was_active = active;
	
	active = true;
	
	if (fontSize <= 0)
		this->fontSize = CONSOLE::DEFAULT_LINE_HEIGHT;
	else
		this->fontSize = fontSize;
	
	// adaptacao
	height -= this->fontSize;
	if (height <= 0.0f)
		height = 1.0f;
	
	const float old_width = this->width;
	const float old_height = this->height;
	//const float old_scale = this->scale;
	
	this->x = x; this->y = y;
	this->width = width; this->height = height;
	this->scale = scale;
	
	scaled_width = this->width * this->scale;
	scaled_height = this->height * this->scale;
	
	if (!was_active)
	{
		
		fade_time = 300; time_on_screen = 0;
		scroll_to_bottom();
		opacity = CONSOLE::MIN_OPACITY;
		scroll_delay = 0;
		
	}
	
	this->always_opaque = always_opaque;
	visible = true;
	
	
	if (text_texture.is_ready() && (old_width != width || old_height != height) )
	{
		text_texture.destroy();
		need_texture_update = true;
	}
	
}



void Console::destroy()
{
	active = false;
}



void Console::reset()
{
	
	Message.clear();
	pendingMessage.clear();
	x = 0; y = 0;
	width = 0; height = 0;
	
	scaled_width = 0;
	scaled_height = 0;
	
	scale = 1.0f;
	fade_time = 0; time_on_screen = 0;
	scroll_position = 0;
	scroll_delay = 0;
	opacity = CONSOLE::MIN_OPACITY;
	always_opaque = false;
	visible = false;
	full_hide = false;
	
	if (text_texture.is_ready() )
		text_texture.destroy();
	
	need_texture_update = false;
	
	inputText.clear();
	visibleInputText.clear();
	
	inputFocus = false;
	need_visible_input_text_update = false;
	show_input_vertical_line = false;
	input_vertical_line_time = 0;
	
}



void Console::hide()
{
	visible = false;
}



void Console::draw()
{
	
	if (!visible) return;
	
	//al_lock_mutex(mutex);
	
	// atualiza a lista de mensagens visiveis de forma thread-safe (assim espero!)
	if (!pendingMessage.empty() )
	{
		
		for (std::vector<ChatString>::iterator it = pendingMessage.begin(); it != pendingMessage.end(); ++it)
			add_message_internal(*it);
		
		pendingMessage.clear();
		
	}
	
	float alpha = opacity;
	if (always_opaque || fade_time <= 0)
		alpha = 1.0f;
	
	if (alpha > 0.0f)
	{
		
		ColorRGBA tint(alpha, alpha, alpha, alpha);
		const float input_box_height = (fontSize + 4.0f) * scale;
		
		// window body
		if (sprite)
		{
			
			if (_9slice_width > 0.0f) // uses 9 slice
			{
			
				const float core_alpha = alpha * 0.5f; //0.95f;
				ColorRGBA core_tint(0.5f * core_alpha, 0.5f * core_alpha, 0.5f * core_alpha, core_alpha); //tint; //(0.0f, 0.0f, 0.0f, core_alpha);
				
				/*
				Bitmap & sprite = window9sliceSprite; //_9sliceSprite
				const float _9slice_width = 8.0f; //10.0f;
				const float _9slice_scale = 1.0f; //0.5f;
				*/
				
				float left, top, _width, _height, input_box_draw_height;
				
				if (borders_outside)
				{
					
					const float scaled_9width = _9slice_width * _9slice_scale;
					
					left = x - scaled_9width;
					top = y - scaled_9width;
					_width = scaled_width + scaled_9width * 2.0f;
					_height = scaled_height + scaled_9width * 2.0f;
					
					input_box_draw_height = input_box_height + scaled_9width;
					
				}
				
				else
				{
					
					left = x;
					top = y;
					_width = scaled_width;
					_height = scaled_height;
					
					input_box_draw_height = input_box_height;
					
				}
				
				const float textbox_top = y + scaled_height;
				const bool input_box_no_top_slices = borders_outside;
				
				// caixa de texto
				spriteCache.draw_9slice_2colors(*sprite, tint, core_tint, left, textbox_top, _width, input_box_draw_height, _9slice_scale, _9slice_width, _9slice_width, input_box_no_top_slices);
				
				// corpo principal
				spriteCache.draw_9slice_2colors(*sprite, tint, core_tint, left, top, _width, _height, _9slice_scale, _9slice_width, _9slice_width);
				
			}
			
			else // simple texture
			{
				
				spriteCache.draw_tinted_sprite_region( *sprite, tint, 0.0f, 0.0f, sprite->get_width(), sprite->get_height(), x, y, scaled_width, scaled_height + input_box_height);
				
			}
			
			spriteCache.flush();
			
		}
		
		
		const int max_visible_messages = get_max_visible_messages();
		const int Message_size = Message.size();
		const float shadow_distance = CONSOLE::RESOLUTION;
		
		if (font)
		{
			
			float font_draw_size = fontSize * CONSOLE::RESOLUTION;
			const float text_scale = font->get_scale_for_height(font_draw_size);
			
			// metodo que usa pre-cache em textura
			if (Message_size > 0)
			{
				
				if (!text_texture.is_ready() )
				{
					if (!text_texture.create( (int)(width * CONSOLE::RESOLUTION), (int)(height * CONSOLE::RESOLUTION), true) )
						printf("Error creating console framebuffer\n");
				}
				
				if (text_texture.is_ready() && need_texture_update)
				{
					
					need_texture_update = false;
					
					text_texture.use();
					clear_screen_to_color( ColorRGBA(0.0f, 0.0f, 0.0f, 0.0f) );
					//clear_screen_to_color( ColorRGBA(0.0f, 0.5f, 0.0f, 0.5f) );
					
					Matrix4 projection = get_orthographic_projection(0.0f, text_texture.get_width(), 0.0f, text_texture.get_height() );
					spriteShader.use();
					spriteShader.setMat4("projview", projection);
					
					// linhas de texto
					int end = max_visible_messages + scroll_position;
					if (end > Message_size) end = Message_size;
					
					int pos = 0;
					
					const float line_offset = font_draw_size;// * 1.4f;
					
					for (int a = scroll_position; a < end; a++)
					{
						
						const float new_x = 10.0f * CONSOLE::RESOLUTION;
						const float new_y = (float)pos * line_offset;// + (line_offset * 0.4f);
						
						float message_x = new_x;
						
						ChatString & c_message = Message[a];
						
						// username
						if (!c_message.user_name.empty() )
						{
							
							std::string display_name = c_message.user_name + ": ";
							message_x += font->get_text_width(display_name, text_scale);
							
							if (shadow_text)
								spriteCache.draw_scaled_text_shadow( *font, display_name, c_message.user_color, BLACK, new_x, new_y, text_scale, text_scale, TEXT_ALIGN_LEFT, shadow_distance);
							else
								spriteCache.draw_scaled_text( *font, display_name, c_message.user_color, new_x, new_y, text_scale, text_scale, TEXT_ALIGN_LEFT);
							
						}
						
						// mensagem
						if (shadow_text)
							spriteCache.draw_scaled_text_shadow( *font, c_message.text, c_message.text_color, BLACK, message_x, new_y, text_scale, text_scale, TEXT_ALIGN_LEFT, shadow_distance);
						else
							spriteCache.draw_scaled_text( *font, c_message.text, c_message.text_color, message_x, new_y, text_scale, text_scale, TEXT_ALIGN_LEFT);
						
						pos += 1;
						
					}
					
					
					spriteCache.flush();
					
					use_default_framebuffer();
					
					// caution!
					Matrix4 base_2D_projection = get_default_2D_projection_matrix();
					spriteShader.setMat4("projview", base_2D_projection);
					
				}
				
				// desenha na tela
				if (text_texture.is_ready() )
				{
					spriteCache.draw_tinted_sprite_region_id( text_texture.get_texture_id(), tint, 0.0f, 0.0f, text_texture.get_width(), text_texture.get_height(), x, y, scaled_width, scaled_height, text_texture.get_width(), text_texture.get_height() );
					spriteCache.flush();
				}
				
			}
			
		}
		
		// indicacao de se tem mensagens alem da area visivel, atualizado sempre devido a detecacao do mouse
		//if (Message_size > max_visible_messages)
		{
			
			#ifdef CONSOLE_SCROLL_GLYPH
			if (font)
			{
				
				const float glyph_size = 20.0f;
				const float right = x + scaled_width - glyph_size;
				const float arrow_scale = scale / CONSOLE::RESOLUTION;
				
				// botao pra cima
				{
					
					const float arrow_y = y + glyph_size * arrow_scale;
					
					ColorRGBA color;
					if (scroll_position > 0)
					{
						
						if (mouse_over_up_button() )
						{
							if (mouse_left_pressed)
								color = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
							else
								color = ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
						}
						else color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
						
					}
					else color = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
					
					color = change_color_opacity(color, alpha);
					
					spriteCache.draw_scaled_glyph( *font, color, right, arrow_y, '^', arrow_scale, arrow_scale);
					
				}
				
				// botao pra baixo
				{
					
					const float arrow_y = y + scaled_height - glyph_size * scale; //- 25.0f * scale;
					
					ColorRGBA color;
					if (scroll_position + max_visible_messages < Message_size )
					{
						
						if (mouse_over_down_button() )
						{
							if (mouse_left_pressed)
								color = ColorRGBA(1.0f, 0.0f, 0.0f, 1.0f);
							else
								color = ColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
						}
						else color = ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
						
					}
					else color = ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f);
					
					color = change_color_opacity(color, alpha);
					
					spriteCache.draw_scaled_glyph( *font, color, right, arrow_y, 'v', arrow_scale, arrow_scale);
					
				}
				
				// cursor de scroll
				const int last_position = Message_size - max_visible_messages;
				const float factor = (float)scroll_position / (float)last_position;
				const float top = y + CONSOLE::BUTTON_WIDTH * scale;
				const float space = scaled_height - (CONSOLE::BUTTON_WIDTH * scale * 2.0f) - (18.0f * scale);
				const float final_y = top + space * factor;
				
				spriteCache.draw_scaled_glyph( *font, ColorRGBA(alpha, alpha, alpha, alpha), right + 1.0f * scale, final_y, '*', arrow_scale, arrow_scale);
				
			}
			#else
				
			// ----versao com sprites
			
			const float button_width = 20.0f * scale;
			const float button_x = x + scaled_width - button_width;
			
			const ColorRGBA pressed_color(0.5f, 0.5f, 0.5f, 1.0f);
			const ColorRGBA hover_color(1.5f, 1.5f, 1.5f, 1.0f);
			const ColorRGBA grayed_color(0.25f, 0.25f, 0.25f, 0.5f);
			
			// fundo da barra
			spriteCache.draw_tinted_sprite_region( whiteSquareBitmap, ColorRGBA(0, 0, 0, 0.5f * alpha), 0.0f, 0.0f, whiteSquareBitmap.get_width(), whiteSquareBitmap.get_height(), button_x, y + button_width, button_width, scaled_height - button_width);
			
			
			// botao pra cima
			{
				
				const float button_y = y;
				
				ColorRGBA color;
				if (scroll_position > 0)
				{
					
					if (mouse_over_up_button() )
					{
						if (mouse_left_pressed)
							color = pressed_color;
						else
							color = hover_color;
					}
					else color = WHITE;
					
				}
				else color = grayed_color;
				
				if (alpha < 1.0f)
					color = change_color_opacity(color, alpha);
				
				spriteCache.draw_tinted_sprite_region( arrowUpSprite, color, 0.0f, 0.0f, arrowUpSprite.get_width(), arrowUpSprite.get_height(), button_x, button_y, button_width, button_width);
				
			}
			
			// botao pra baixo
			{
				
				const float button_y = y + scaled_height - button_width;
				
				ColorRGBA color;
				if (scroll_position + max_visible_messages < Message_size )
				{
					
					if (mouse_over_down_button() )
					{
						if (mouse_left_pressed)
							color = pressed_color;
						else
							color = hover_color;
					}
					else color = WHITE;
					
				}
				else color = grayed_color;
				
				if (alpha < 1.0f)
					color = change_color_opacity(color, alpha);
				
				spriteCache.draw_tinted_sprite_region( arrowDownSprite, color, 0.0f, 0.0f, arrowDownSprite.get_width(), arrowDownSprite.get_height(), button_x, button_y, button_width, button_width);
				
			}
			
			// cursor de scroll
			if (Message_size > max_visible_messages)
			{
				
				const int last_position = Message_size - max_visible_messages;
				const float factor = (float)scroll_position / (float)last_position;
				const float top = y + button_width;
				const float space = scaled_height - (button_width * 2.0f) - (18.0f * scale);
				const float final_y = top + space * factor;
				
				spriteCache.draw_tinted_sprite_region( scrollBoxSprite, ColorRGBA(alpha, alpha, alpha, alpha), 0.0f, 0.0f, scrollBoxSprite.get_width(), scrollBoxSprite.get_height(), button_x, final_y, button_width, button_width);
				
			}
			
			#endif
			
		}
		
		
		
		// --- campo de texto (a caixa eh desenhada acima no comeco)
		if (font)
		{
			
			float textHeight = fontSize * scale;
			const float input_text_scale = font->get_scale_for_height(textHeight);
			
			const float text_x = round_to_nearest(x + 5.0f * scale);
			const float text_y = round_to_nearest( y + scaled_height + (input_box_height - textHeight) * 0.5f); 
			
			std::string display_string;
			//glm::vec4 display_color = text_color;
			
			if (!inputText.empty() )
			{
				
				if (need_visible_input_text_update)
					update_visible_input_text();
				
				display_string = visibleInputText;
				
			}
			
			if (inputFocus && show_input_vertical_line)
				display_string += "|";
			
			/*
			else if (!default_text.empty() && !focus) // mostra texto apagado caso esteja vazio e sem foco
			{
				display_string = default_text;
				//display_color = change_color_opacity(text_color, 0.5f);
			}
			else
			{
				display_string = cursor();
			}*/
			
			// finalmente, escreve o texto
			if (!display_string.empty() )
			{
				if (shadow_text)
					spriteCache.draw_scaled_text_shadow( *font, display_string, tint, ColorRGBA(0.0f, 0.0f, 0.0f, alpha), text_x, text_y, input_text_scale, input_text_scale, TEXT_ALIGN_LEFT, 1);
				else
					spriteCache.draw_scaled_text( *font, display_string, tint, text_x, text_y, input_text_scale, input_text_scale, TEXT_ALIGN_LEFT);
			}
			
		}
		
		spriteCache.flush();
		
	}
	
	//al_unlock_mutex(mutex);
	
}



void Console::clear_messages()
{
	
	//al_lock_mutex(mutex);
	
	Message.clear();
	pendingMessage.clear();
	scroll_position = 0;
	
	//al_unlock_mutex(mutex);
	
}



void Console::add_message(const ChatString & message)
{
	
	//al_lock_mutex(mutex);
	pendingMessage.push_back(message);
	//al_unlock_mutex(mutex);
	
}


// private
void Console::add_message_internal(const ChatString & message)
{
	
	if (!font)
	{
		static bool once = false;
		if (!once)
		{
			once = true;
			printf("Set a font for console before adding messages\n");
		}
		return;
	}
	
	const int available_width = this->width - 10 * 2 - 20;
	//const int right_offset = 30;
	const int max_visible_messages = get_max_visible_messages();
	
	const int temp_Message_size = Message.size();
	const bool cursor_at_bottom = (temp_Message_size < max_visible_messages || temp_Message_size - scroll_position == max_visible_messages);
	
	std::string user_name = message.user_name + ": ";
	const float font_scale = font->get_scale_for_height(fontSize);
	int username_width = (float)font->get_text_width(user_name, font_scale);// / CONSOLE::RESOLUTION;
	int text_width = (float)font->get_text_width(message.text, font_scale);// / CONSOLE::RESOLUTION;
	int lines_added = 0;
	
	
	
	bool endline_char_exists = (message.text.find('\n') != std::string::npos);
	
	// uma linha
	const int total_width = username_width + text_width;
	if (!endline_char_exists && (message.text.empty() || this->width <= 0 || total_width <= available_width) )
	{
		Message.push_back(message);
		lines_added = 1;
		
		//printf("added 1 line. width: %d\n", total_width);
	}
	
	else // multi linha
	{
		
		bool first_line = true;
		int start_index = 0;
		int last_space_pos = -1; // indice do ultimo espaco encontrado
		std::string line;
		
		const int text_lenght = message.text.size();
		const int final_index = text_lenght - 1;
		for (int pos = 0; pos < text_lenght; pos++)
		{
			
			char current_char = message.text[pos];
			
			if (current_char == ' ')
				last_space_pos = pos;
			
			if (current_char != '\n')
				line += current_char;
			
			//const int expected_width = ( (float)font->get_text_width(line, font_scale) / CONSOLE::RESOLUTION) + (first_line ? username_width : 0);
			const int expected_width = (float)font->get_text_width(line, font_scale) + (first_line ? username_width : 0);
			const bool reached_end = (pos == final_index);
			const bool overflow = expected_width > available_width;
			
			// se chegou no fim ou excedeu o comprimento maximo
			if (current_char == '\n' || reached_end || overflow)
			{
				
				// se excedeu, corta pro fim da ultima palavra
				if (overflow && current_char != ' ' && last_space_pos > 0)
				{
					
					const int line_lenght = last_space_pos - start_index;
					
					if (line_lenght > 0 && start_index >= 0 && start_index <= final_index)
					{
						line = std::string(message.text, start_index, line_lenght);
						pos = last_space_pos;
					}
					
				}
				
				
				std::string new_username;
				
				// somente a primeira linha mostra o nick
				if (first_line)
				{
					
					new_username = message.user_name;
					first_line = false;
					
				}
				
				else
				{
					new_username = "";
				}
				
				Message.push_back( ChatString( line, new_username, message.user_color, message.text_color) );
				
				//printf("added multi line. width: %d\n", expected_width);
				
				lines_added += 1;
				start_index = pos + 1;
				last_space_pos = -1; // necessario para evitar bug
				
				line.clear();
				
			}
			
		}
		
	}
	
	
	//Message.push_back(message);
	
	// tempo = constante + valor proporcional ao comprimento do texto
	time_on_screen = fade_time;
	//opacity = 1.0f;
	
	if (cursor_at_bottom && (int)Message.size() > max_visible_messages)
		scroll_down(lines_added);
	
	need_texture_update = true;
	
}



int Console::update(bool ignore_inputs)
{
	
	//al_lock_mutex(mutex);
	
	const bool input_key_pressed = key_down[KEY_ENTER] || key_down[KEY_KEYPAD_ENTER];
	
	const bool ignore_interaction = (ignore_inputs || (full_hide && !always_opaque && opacity <= 0.0f && !input_key_pressed) );
	
	const bool mouse_is_inside = mouse_inside();
	
	if (time_on_screen > 0 || (mouse_is_inside && !ignore_interaction) ) // fadein
	{
		
		if (opacity < 1.0f)
		{
			opacity += CONSOLE::FADE_SPEED;
			if (opacity > 1.0f) opacity = 1.0f;
		}
		
	}
	
	else if (!inputFocus) // fadeout
	{
		
		const float min_opacity = full_hide ? 0.0f : CONSOLE::MIN_OPACITY;
		if (opacity > min_opacity)
		{
			opacity -= CONSOLE::FADE_SPEED;
			if (opacity < min_opacity) opacity = min_opacity;
		}
		
	}
	
	if (time_on_screen > 0)
		time_on_screen--;
	
	if (scroll_delay > 0) // delay pra scrollar as mensagens
		scroll_delay--;
	else
	{
		
		if (!ignore_interaction && mouse_is_inside)
		{
			
			if (mouse_wheel_up || up_button_pressed() )
				scroll_up();
				
			else if (mouse_wheel_down || down_button_pressed() )
				scroll_down();
			
			
			if (key_down[KEY_PAGE_UP] )
				scroll_to_top();
			
			else if (key_down[KEY_PAGE_DOWN] )
				scroll_to_bottom();
			
		}
		
	}
	
	// text field
	if (mouse_left_down && !ignore_inputs)
	{
	
		// pega foco
		if (mouse_is_inside)
		{
			inputFocus = true;
			last_keychar = 0;
		}
		
		else if (inputFocus)
			inputFocus = false;
		
	}
	
	if (inputFocus)
	{
		
		check_copy_paste();
		read_text_input();
		
		input_vertical_line_time--;
		if (input_vertical_line_time <= 0)
		{
			show_input_vertical_line = !show_input_vertical_line;
			input_vertical_line_time = 30;
		}
		
		
		// retorna 1 quando conclui a digitacao
		if (input_key_pressed)
		{
			if (!inputText.empty() )
				return 1;
			
			else if (!always_opaque)
				inputFocus = false;
		}
		
	}
	
	else if (input_key_pressed && !always_opaque)
	{
		inputFocus = true;
		opacity = 1.0f;
	}
	
	//al_unlock_mutex(mutex);
	
	return 0;
	
}



void Console::scroll_up()
{
	
	if (scroll_position > 0)
	{
		
		scroll_position--;
		scroll_delay = CONSOLE::SCROLL_DELAY;
		
		need_texture_update = true;
		
	}
	
}



void Console::scroll_down(int lines)
{
	
	const int scroll_limit = (int)Message.size() - get_max_visible_messages();
	for (int a = 0; a < lines; a++)
	{
		
		if (scroll_position < scroll_limit)
		{
			
			scroll_position++;
			scroll_delay = CONSOLE::SCROLL_DELAY;
			
			need_texture_update = true;
			
		}
		else break;
	
	}
	
}



void Console::scroll_to_top()
{
	
	if (scroll_position != 0)
	{
		scroll_position = 0;
		need_texture_update = true;
	}
	
}


void Console::scroll_to_bottom()
{
	
	const int old_position = scroll_position;
	
	const int max_visible_messages = get_max_visible_messages();
	const int Message_size = Message.size();
	if (Message_size >= max_visible_messages)
		scroll_position = Message_size - max_visible_messages;
	else 
		scroll_position = 0;
	
	if (old_position != scroll_position)
		need_texture_update = true;
	
}



int Console::get_max_visible_messages()
{
	return (int)( (height - fontSize) / fontSize);
}


bool Console::is_empty()
{
	return (Message.empty() && pendingMessage.empty() );
}


bool Console::mouse_inside()
{
	//return mouse_inside_rectangle(x, y, scaled_width, scaled_height);
	float _x, _y, _width, _height;
	get_dimensions(_x, _y, _width, _height);
	return mouse_inside_rectangle(_x, _y, _width, _height);
	
}



bool Console::currently_visible()
{
	return (visible && (opacity > 0.0f || always_opaque || fade_time <= 0) );
}



bool Console::mouse_over_up_button()
{
	
	const float button_width = CONSOLE::BUTTON_WIDTH * scale;
	const float left = (float)(x + scaled_width - 1.0f - button_width);
	return mouse_inside_rectangle(left, y + 1.0f, button_width, button_width);
	
}

bool Console::mouse_over_down_button()
{
	
	const float button_width = CONSOLE::BUTTON_WIDTH * scale;
	const float left = x + scaled_width - 1.0f - button_width;
	const float top = y + scaled_height - 1.0f - button_width;
	return mouse_inside_rectangle(left, top, button_width, button_width);
	
}


bool Console::up_button_pressed()
{
	return (mouse_left_pressed && mouse_over_up_button() );
}


bool Console::down_button_pressed()
{	
	return (mouse_left_pressed && mouse_over_down_button() );	
}




void Console::check_copy_paste()
{
	
	if (key_pressed[KEY_LEFT_CONTROL] || key_pressed[KEY_RIGHT_CONTROL])
	{
		
		if (key_down[KEY_C])
		{	
			
			copy_to_clipboard(inputText);
			last_keychar = 0;
			
		}
		
		else if (key_down[KEY_V])
		{	
			
			// todo: previnir paste gigante
			
			inputText += paste_from_clipboard();
			need_visible_input_text_update = true;
			
			last_keychar = 0;
			
		}
		
	}
	
}



void Console::read_text_input()
{
	
	int result = ::read_text_input(inputText, 0, true, 1000, 0.0f, NULL);
	if (result != 0)
	{
		need_visible_input_text_update = true;
	}
	
}



void Console::update_visible_input_text()
{
	
	if (!font) return;
	
	need_visible_input_text_update = false;
	
	visibleInputText.clear();
	
	const int text_size = inputText.size();
	if (text_size > 0)
	{
		
		float font_draw_size = fontSize * scale;
		const float text_scale = font->get_scale_for_height(font_draw_size);
		
		const float end_offset = (float)font->get_character_width('|') + 10.0f;
		
		std::string comparison_string;
		
		int pos;
		for (pos = text_size - 1; pos >= 0; pos--) // vai do fim ao inicio
		{
			
			char current = inputText[pos];
			
			comparison_string.insert(0, 1, current);
			
			// deteccao de multibyte char
			if (current < 0 && pos > 0) 
			{
				
				comparison_string.insert(0, 1, inputText[pos - 1]);
				pos -= 1;
				
			}
			
			// otimizar pra calcular passo a passo!
			const float predicted_width = (float)font->get_text_width(comparison_string) + end_offset;
			if (predicted_width * text_scale > scaled_width) 
			{
				pos++; // volta um pra nao vazar
				break; // para aqui!
			}
			
		}
		
		// define a string com base no intervalo
		if (pos < 0) pos = 0;
		if (pos >= text_size) pos = text_size - 1;
		
		visibleInputText = std::string(inputText, pos);
	
	}
	
}


float Console::get_height()
{
	
	if (_9slice_width > 0.0f && borders_outside)
	{
		return scaled_height + get_input_box_height() + _9slice_width * 2.0f;
	}
	return scaled_height + get_input_box_height();
	
}



float Console::get_input_box_height()
{
	return fontSize * scale;
}



void Console::get_dimensions(float & x, float & y, float & width, float & height)
{
	
	if (_9slice_width > 0.0f && borders_outside)
	{
		
		x = this->x - _9slice_width;
		y = this->y - _9slice_width;
		width = scaled_width + _9slice_width * 2.0f;
		height = scaled_height + _9slice_width * 2.0f + get_input_box_height();
		
	}
	
	else
	{
		
		x = this->x;
		y = this->y;
		width = this->scaled_width;
		height = this->scaled_height + get_input_box_height();
		
	}
	
}



void Console::get_bounds(float & left, float & top, float & right, float & bottom)
{
	
	if (_9slice_width > 0.0f && borders_outside)
	{
		
		left = x - _9slice_width;
		top = y - _9slice_width;
		right = x + scaled_width + _9slice_width;
		bottom = y + scaled_height + _9slice_width + get_input_box_height();
		
	}
	
	else
	{
		
		left = x;
		top = y;
		right = x + scaled_width;
		bottom = y + scaled_height + get_input_box_height();
		
	}
	
}

/*
std::vector<ChatString> Message;
std::vector<ChatString> pendingMessage;

Framebuffer text_texture;
bool need_texture_update;

//ALLEGRO_MUTEX * mutex;


void add_message_internal(const ChatString & message);

public:
int fade_time, time_on_screen;
int scroll_position;
char scroll_delay;
float opacity;
bool always_opaque;
bool visible;
bool full_hide;

std::string inputText, visibleInputText;
bool inputFocus;
bool need_visible_input_text_update;
bool show_input_vertical_line;
int8_t input_vertical_line_time;

// customization
Bitmap * sprite;
float _9slice_scale;
float _9slice_width;
Font * font;
bool shadow_text;
bool borders_outside;
*/


void Console::debug_printf()
{
	
	printf("active: %s\n", active ? "true" : "false");
	printf("x: %.2f, y: %.2f\n", x, y);
	printf("width: %.2f, height: %.2f\n", width, height);
	printf("scaled_width: %.2f, scaled_height: %.2f\n", scaled_width, scaled_height);
	printf("scale: %.2f\n", scale);
	printf("fontSize: %d\n", fontSize);
	
	printf("get_max_visible_messages(): %d\n", get_max_visible_messages() );
	
}


