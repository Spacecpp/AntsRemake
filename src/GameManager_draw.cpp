#include "GameManager.h"
#include <string>
#include "interface_list.h"
#include "Colors.h"

void GameManager::draw()
{
	
	if (gameState == GAMESTATE::EXIT) return;
	
	
	clear_screen_to_color( ColorRGBA(0.0f, 0.0f, 0.0f, 1.0f) );
	
	spriteShader.use();
	
	// screen space
	Matrix4 base_2D_projection = get_default_2D_projection_matrix();
	Matrix4 base_view(1.0f); 
	Matrix4 base_projview = base_2D_projection * base_view;
	spriteShader.setMat4("projview", base_projview);
	
	
	
	
	if (gameState == GAMESTATE::MENU)
		draw_menu();
	
	else if (gameState == GAMESTATE::PLAYING)
    {
		draw_gameplay();
        spriteShader.setMat4("projview", base_projview);
        draw_interface();
    }
    
    
    if (mapEditor_upperMenu.active)
		mapEditor_upperMenu.draw();
		
	if (mapEditor_toolWindow.active)
		mapEditor_toolWindow.draw();	
		
	if (_GUI_Window.active)
		_GUI_Window.draw();
		
	
	// pos GUI draw
	if (gameState == GAMESTATE::MENU)
	{
		
		if (_GUI_Window.active && _GUI_Window.code == WINDOW_ID::GAME_SETUP) //if (inGameSetupScreen)
			draw_game_setup_menu();
		
		if (console.active)
			console.draw();
		
	}
	
	
	if (show_debug)
	{
		
		const float font_size = 32.0f;
		
		std::string str = "GF: " + to_stringi(gameFrame) + " / " + to_stringi(gameFrameMax);
		if (gameFrameDone)
            str += " (done)";
            
		ColorRGBA color = (gameFrame >= gameFrameMax && gameFrameDone) ? RED : WHITE;
        spriteCache.draw_text_height(the_font, str, color, screen_w * 0.5f, 10.0f, font_size, TEXT_ALIGN_LEFT);
        
        if (is_online() )
        {
        	
            std::string networkFrame_str = "networkFrame: " + to_stringi(networkFrame);
            if (networkFrame > gameFrame)
                networkFrame_str += " (" + to_stringi(networkFrame - gameFrame) + " frames remaining)";
                
            spriteCache.draw_text_height(the_font, networkFrame_str, WHITE, screen_w * 0.5f, 30.0f, font_size, TEXT_ALIGN_LEFT);
            
        }
	
		int queuedOrder_count = queuedOrder.size();
		int netQueuedOrder_count = netQueuedOrder.size();
		//if (queuedOrder_count > 0 || netQueuedOrder_count > 0)
		{
			
			std::string str1 = "Orders: " + to_stringi(queuedOrder_count);
			spriteCache.draw_text_height(the_font, str1, WHITE, screen_w * 0.5f, 50.0f, font_size, TEXT_ALIGN_LEFT);
			
			std::string str2 = "Net Orders: " + to_stringi(netQueuedOrder_count);
			spriteCache.draw_text_height(the_font, str2, WHITE, screen_w * 0.5f, 70.0f, font_size, TEXT_ALIGN_LEFT);
			
		}
		
		
		spriteCache.draw_text_height(the_font, console.active ? "console.active" : "console not active", WHITE, 10, 50.0f, font_size, TEXT_ALIGN_LEFT);
		
		
		uint32_t seed = gameRNG.get_state_as_integer();
		spriteCache.draw_text_height(the_font, to_stringi(seed), WHITE, 10, 100.0f, font_size, TEXT_ALIGN_LEFT);
		
	}
	
	if (simulateConnectionLost)
        spriteCache.draw_text_height(the_font, "Fake connection lost", RED, screen_w * 0.5f, 30.0f, 32.0f, TEXT_ALIGN_CENTER);
	
	spriteCache.flush();
	
}



void GameManager::draw_menu()
{
	
	// background
    const float screen_width = screen_w;
	const float screen_height = screen_h;
	spriteCache.draw_sprite_region(menuBackgroundTexture, 0.0f, 0.0f, screen_width, screen_height, 0.0f, 0.0f, screen_width, screen_height);
	
	// logo
	if (_GUI_Window.active && _GUI_Window.code == WINDOW_ID::MAIN_MENU)
	{
		
		const float offset = 10.0f;
		const float draw_height = (_GUI_Window.y - abs(_GUI_Window.titlebar_height) ) - offset * 2.0f;
		const float src_heigth = logoSprite.get_height();
		
		if (draw_height > 0.0f && src_heigth > 0.0f)
		{
			
			const float scale_factor = draw_height / src_heigth;
			const float src_width = logoSprite.get_width();
			const float draw_width = src_width * scale_factor;
			const float draw_x = screen_w * 0.5f - draw_width * 0.5f;
			const float draw_y = offset;
			
			spriteCache.draw_sprite_region(logoSprite, 0.0f, 0.0f, src_width, src_heigth, draw_x, draw_y, draw_width, draw_height);
			
		}
		
		// texto de compilacao
		String str = "Compiled " + String(__DATE__);
		spriteCache.draw_text_height(the_font, str, WHITE, 20, screen_h - 40, 32.0f, TEXT_ALIGN_LEFT);
		
	}
	
}



void GameManager::draw_game_setup_menu()
{
	
	// caixa com os players
	//float font_size = 32.0f;
	
	//float cursor_x = _GUI_Window.x + 20.0f;
	//float cursor_y = _GUI_Window.y + 20.0f;
	
	//spriteCache.draw_9slice(window9sliceInsideSprite, WHITE, cursor_x - 4, cursor_y, 256, 64 * 4 + 10, 2.0f, 4, 4);
	
	float cursor_x = 0, cursor_y = 0;
	if (!_GUI_Window.image.empty() )
	{
		cursor_x = _GUI_Window.image[0].x;
		cursor_y = _GUI_Window.image[0].y;
	}
	
	for (Player & c_Player : player)
	{
		
		if (c_Player.active)
		{
			
			ColorRGBA color = c_Player.get_color();
			
			// ant
			{
				
				Bitmap * sprite = &antSprite;
				
				const float src_width = sprite->get_width() / ANT_ANIMATION_FRAMES;
				const float src_height = sprite->get_height() / 2;
				
				const float src_x = src_width * menuAntAnimationFrame;
				
				const float draw_width = src_width * 2.0f;
				const float draw_height = src_height * 2.0f;
				
				const float half_width = draw_width * 0.5f;
				const float half_height = draw_height * 0.5f;
				
				const float draw_x = cursor_x + draw_width * 0.5f;
				const float draw_y = cursor_y + draw_height * 0.5f;
				
				spriteCache.draw_tinted_rotated_sprite_region(*sprite, color, half_width, half_height, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height, 0.0f);
				
				spriteCache.draw_tinted_rotated_sprite_region(*sprite, WHITE, half_width, half_height, src_x, src_height, src_width, src_height, draw_x, draw_y, draw_width, draw_height, 0.0f);
				
			}
			
			
			// name
			//textCache.draw_text_height(the_font, c_Player.name, color, cursor_x + 64 + 10, cursor_y + 20, font_size, TEXT_ALIGN_LEFT);
			
			
			// ready
			if (c_Player.ready)
				spriteCache2.draw_sprite(readyCheckmarkSprite, cursor_x + 200, cursor_y + 20);
			
			
			cursor_y += 64.0f;
			
		}
		
	}
	
	
	spriteCache.flush();
	spriteCache2.flush();
	textCache.flush();
	
	// ----
	
	cursor_x = _GUI_Window.x + _GUI_Window.width - 300.0f;
	if (!_GUI_Window.textBox.empty() )
		cursor_y = _GUI_Window.textBox.back().y + 40.0f;
	//_GUI_Window.y + 70.0f + 30.0f; // fazer pegar embaixo do ultimo textbox
	
	if (gameMap.loaded)
		draw_minimap(cursor_x, cursor_y, false);
	
	/*
	std::string map_str = "Map: " + selectedMapName;
	//DrawTextPro_aligned(the_font, map_str.c_str(), Vector2 { cursor_x, cursor_y}, Vector2 { 0.0f, 0.0f }, 0, font_size, 0, WHITE, TEXT_ALIGN_RIGHT);
	spriteCache.draw_text_height(the_font, map_str, WHITE, cursor_x, cursor_y, font_size, TEXT_ALIGN_RIGHT);
	cursor_y += 30.0f;
	
	std::string cd_str = "Command delay: " + std::to_string(commandDelay);
	//DrawTextPro_aligned(the_font, cd_str.c_str(), Vector2 { cursor_x, cursor_y}, Vector2 { 0.0f, 0.0f }, 0, font_size, 0, WHITE, TEXT_ALIGN_RIGHT);
	spriteCache.draw_text_height(the_font, cd_str, WHITE, cursor_x, cursor_y, font_size, TEXT_ALIGN_RIGHT);
	*/
	
}



void GameManager::draw_gameplay()
{
	
	// fundo
	const float screen_width = screen_w;
	const float screen_height = screen_h;
	spriteCache.draw_sprite_region(translucentSquaresSprite, 0.0f, 0.0f, screen_width, screen_height, 0.0f, 0.0f, screen_width, screen_height);
	spriteCache.flush();
	
	//BeginMode2D(camera);
	
	// world space
	Matrix4 base_2D_projection = get_default_2D_projection_matrix();
    Matrix4 camera_view = camera.get_view_matrix();
    Matrix4 camera_projview = base_2D_projection * camera_view;
    spriteShader.setMat4("projview", camera_projview);
	
	
	draw_map();
	
	// marca tiles solidos
	if (show_map_debug)
	{
		for (int y = 0; y < gameMap.height; y++) 
		{
			for (int x = 0; x < gameMap.width; x++) 
			{
				if (gameMap.tile_is_solid(x, y) )
				{
					spriteCache.draw_tinted_sprite_region(whiteSquareBitmap, ColorRGBA(0.0f, 0.5f, 0.0f, 0.0f), 0, 0, whiteSquareBitmap.get_width(), whiteSquareBitmap.get_height(), x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
				}
			}
		}
	}
	
	/*
	if (editMode)
		draw_editor_elements();
	else*/
		draw_gameplay_elements();
		
	
	// preview do que vai ser colocado no editor
	if (editMode)
	{
		// TODO
		
	}
	
	
	// bounding box
	if (show_map_debug)
	{
		
		spriteCache.flush();
		spriteCache.set_texture(whiteSquareBitmap);
		
		const float line_thickness = 1.0f / camera.zoom;
		
		for (Obstacle & current : obstacle)
		{
			if (current.active)
				current.debug_draw_collision_box(line_thickness);
		}
		
		for (Food & current : food)
		{
			if (current.active)
				current.debug_draw_collision_box(line_thickness);
		}
		
		spriteCache.flush();
		
	}
	
	
	// caixa de selecao
	if (draggingBoxSelection)
	{
		
		if (boxSelection.z != 0.0f && boxSelection.w != 0.0f)
		{
			
			float x = boxSelection.x;
			float y = boxSelection.y;
			float width = boxSelection.z;
			float height = boxSelection.w;
			
			if (width < 0.0f)
			{
				width = -width;
				x -= width;
			}
			
			if (height < 0.0f)
			{
				height = -height;
				y -= height;
			}
			
			spriteCache.flush();
			spriteCache.set_texture(whiteSquareBitmap);
			const float line_width = 1.0f / camera.zoom;
			spriteCache.draw_rectangle(x, y, x + width, y + height, WHITE, line_width); 
			
		}
		
	}
	
	spriteCache.flush();
	
}



void GameManager::draw_map()
{
	
	// plano de agua por baixo de tudo
	const float map_world_width = (float)(gameMap.width * TILE_SIZE);
	const float map_world_height = (float)(gameMap.height * TILE_SIZE);
	
	spriteCache.draw_sprite_region(waterTexture, waterAnimationOffset, waterAnimationOffset, map_world_width, map_world_height, 0.0f, 0.0f, map_world_width, map_world_height);
	spriteCache.flush(); // desenha antes dos tiles, que tem cache proprio
	
	// tiles em si
	gameMap.draw();
	
	// constantes pre calculadas
	const float fireSpriteWidth = fireSprite.get_width() / global_fireAnimationFrames;
	const float mineSpriteWidth = landMineSprite.get_width() / 2;
	const float mineSpriteHeight = landMineSprite.get_height() / 2;
	
	// objetos atrelados aos tiles
	for (int y = 0; y < gameMap.height; y++) 
	{
		for (int x = 0; x < gameMap.width; x++) 
		{
			
			int8_t secondary = gameMap.terrain[y][x].secondary;
			if (secondary == SECONDARY_TILE::FIRE) // fire field
			{
				
				const float draw_x = x * (float)TILE_SIZE - ( (fireSpriteWidth - 32.0f) * 0.5f);
				const float draw_y = y * (float)TILE_SIZE - (fireSpriteWidth - 32.0f);
				
				const float src_x = (float)clamp_wrap(fireAnimationFrame + x + y, 0, global_fireAnimationFrames - 1) * fireSpriteWidth;
				
				spriteCache.draw_sprite_region(fireSprite, src_x, 0.0f, fireSpriteWidth, fireSpriteWidth, draw_x, draw_y, fireSpriteWidth, fireSpriteWidth);
				
			}
			
			else if (secondary == SECONDARY_TILE::MUD_BRIDGE) // bridge
			{
				
				const float draw_x = x * (float)TILE_SIZE;
				const float draw_y = y * (float)TILE_SIZE;
				
				//const float src_x = (float)clamp_wrap(fireAnimationFrame + x + y, 0, 2) * 32.0f;
				
				spriteCache.draw_sprite_region(mudBridgeSprite, 0.0f, 0.0f, 32.0f, 32.0f, draw_x, draw_y, 32.0f, 32.0f);
				
			}
			
			else if (secondary >= SECONDARY_TILE::LANDMINE_0 && secondary < SECONDARY_TILE::LANDMINE_0 + MAX_PLAYERS) // mine
			{
				
				const float draw_x = x * (float)TILE_SIZE;
				const float draw_y = y * (float)TILE_SIZE;
				
				int index = secondary - SECONDARY_TILE::LANDMINE_0;
				ColorRGBA tint = (index >= 0 && index < (int)player.size() ) ? player[index].get_bright_color() : WHITE;
				
				// 1 part
				//spriteCache.draw_tinted_sprite_region(landMineSprite, tint, 0.0f, 0.0f, 32.0f, 32.0f, draw_x, draw_y, 32.0f, 32.0f);
				
				const int frame = (mineAnimationFrame + x + y) % 2;
				const float src_x = frame * mineSpriteWidth;
				
				spriteCache.draw_tinted_sprite_region(landMineSprite, tint, src_x, 0.0f, mineSpriteWidth, mineSpriteHeight, draw_x, draw_y, mineSpriteWidth, mineSpriteHeight);
				spriteCache.draw_tinted_sprite_region(landMineSprite, WHITE, src_x, mineSpriteHeight, mineSpriteWidth, mineSpriteHeight, draw_x, draw_y, mineSpriteWidth, mineSpriteHeight);
				
			}
			
		}
	}
	
}



void GameManager::draw_gameplay_elements()
{
	
	if (editMode)
	{
		for (const PowerupGenerator & c_PowerupGenerator : powerupGenerator) 
		{
			if (c_PowerupGenerator.active)
				c_PowerupGenerator.draw_editor_indicator();
		}
		
		for (const PowerupGenerator & c_PowerupGenerator : powerupGenerator) 
		{
			if (c_PowerupGenerator.active)
				c_PowerupGenerator.draw_editor_time();
		}
	}
	
	
	for (const AntHill & c_antHill : antHill) 
	{
		if (c_antHill.active)
			c_antHill.draw();
	}
	
	for (const Powerup & c_powerup : powerup) 
	{
		if (c_powerup.active)
			c_powerup.draw();
	}
	
	draw_food();
	
	for (const Obstacle & entity : obstacle) 
	{
		if (entity.active)
			entity.draw();
	}
	
	for (const AnimatedSprite & c_animatedSprite : animatedSprite) 
	{
		if (c_animatedSprite.active)
			c_animatedSprite.draw();
	}
	
	for (PowerupGenerator & c_powerupGenerator : powerupGenerator) 
	{
		if (c_powerupGenerator.active)
			c_powerupGenerator.draw_droplet();
	}
	
	draw_ants();
	
	// flags
	for (const AntHill & c_antHill : antHill) 
	{
		if (c_antHill.active)
			c_antHill.draw_flag();
	}
	
	
	for (const FlyingText & current : flyingText) 
	{
		if (current.active)
			current.draw();
	}
	
	for (const HealParticle & current : healParticle) 
	{
		if (current.active)
			current.draw();
	}
	
	
	if (debug_showPaths)
	{
	    
		spriteCache.flush();
		spriteCache.set_texture(whiteSquareBitmap);
		for (Ant & c_ant : ant) 
		{
			if (c_ant.active && !c_ant.path.empty() )
				c_ant.draw_path();
		}
		
	}
	
}


/*
void GameManager::draw_editor_elements()
{
	
	// anthill
	for (const IntVector3 & current : gameMap.antHillSpawn) 
	{
		
		Bitmap * sprite = &antHillDefaultSprite;
		
		const float src_width = sprite->get_width();
		const float src_height = sprite->get_height();
		//const float half_width = src_width * 0.5f;
		//const float half_height = src_height * 0.5f;
		
		const float draw_width = src_width;
        const float draw_height = src_height;
		
		const float draw_x = (float)current.x * TILE_SIZE + TILE_SIZE * 0.5f - draw_width * 0.5f;
		const float draw_y = (float)current.y * TILE_SIZE + TILE_SIZE * 0.5f - draw_height * 0.5f;
		
		spriteCache.draw_tinted_sprite_region(*sprite, get_indexed_color(current.z), 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
		
	}
	
	// food
	for (const IntVector3 & current : gameMap.foodSpawn) 
	{
		
		//Bitmap * sprite = get_food_sprite(current.z); //&donutSprite;
		FoodData * data = get_food_data(current.z);
		if (data && data->sprite.is_ready() )
		{
            
            Bitmap & sprite = data->sprite;
            
            const int total_eat_frames = std::max( (int)data->eatFrames, 1);
			const int total_variants = std::max( (int)data->variants, 1);
			const int total_animation_frames = std::max( (int)data->animationFrames, 1);
			
			const float base_src_width = sprite.get_width();
			const float base_src_height= sprite.get_height();
			
			const float variant_src_width = base_src_width / (float)total_variants;
			
			const float src_width = variant_src_width / (float)total_eat_frames;
			const float src_height = base_src_height / (float)total_animation_frames;
			
			//const float src_x = (float)get_current_eat_frame() * src_width + variantIndex * variant_src_width;
			//const float src_y = (float)animationFrame * src_height;
			
			const float draw_width = src_width;
			const float draw_height = src_height;
			
			const float draw_x = (float)current.x * TILE_SIZE + TILE_SIZE * 0.5f - draw_width * 0.5f;
			const float draw_y = (float)current.y * TILE_SIZE + TILE_SIZE * 0.5f - draw_height * 0.5f;
            
            spriteCache.draw_sprite_region(data->sprite, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
		}
		
	}
	
	// obstacle
	for (const IntVector3 & current : gameMap.obstacleSpawn) 
	{
		
		ObstacleData * data = get_obstacle_data(current.z);
		if (data && data->sprite.is_ready() )
		{
			
            Bitmap & sprite = data->sprite;
            
			const int total_variants = std::max( (int)data->variants, 1);
			const int total_animation_frames = std::max( (int)data->animationFrames, 1);
			
			const float base_src_width = sprite.get_width();
			const float base_src_height= sprite.get_height();
			
			const float variant_src_width = base_src_width / (float)total_variants;
			
			const float src_width = variant_src_width;
			const float src_height = base_src_height / (float)total_animation_frames;
			
			const float draw_width = src_width;
			const float draw_height = src_height;
			
			const float draw_x = (float)current.x * TILE_SIZE + TILE_SIZE * 0.5f - draw_width * 0.5f;
			const float draw_y = (float)current.y * TILE_SIZE + TILE_SIZE * 0.5f - draw_height * 0.5f;
            
            spriteCache.draw_sprite_region(data->sprite, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
            
		}
		
	}
	
	// powerup
	for (const IntVector3 & current : gameMap.itemSpawn) 
	{
		
		Bitmap * sprite = &powerupSprite;
		
		const float src_width = sprite->get_height();
		const float src_height = sprite->get_height();
		//const float half_width = src_width * 0.5f;
		//const float half_height = src_height * 0.5f;
		const float draw_width = src_width;
        const float draw_height = src_height;
        
		const float src_x = (float)(current.z - 1) * src_width;
		
		const float draw_x = (float)current.x * TILE_SIZE + TILE_SIZE * 0.5f - draw_width * 0.5f;
		const float draw_y = (float)current.y * TILE_SIZE + TILE_SIZE * 0.5f - draw_height * 0.5f;
		
		spriteCache.draw_sprite_region(*sprite, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
		
	}
	
	// ant
	for (const IntVector3 & current : gameMap.antSpawn) 
	{
		
		Bitmap * sprite = &antSprite;
		
		const float src_width = sprite->get_width() / ANT_ANIMATION_FRAMES;
		const float src_height = sprite->get_height() / 2;
		
		const float draw_width = src_width;
        const float draw_height = src_height;
        
        const float half_width = draw_width * 0.5f;
        const float half_height = draw_height * 0.5f;
		
		const float draw_x = (float)current.x * TILE_SIZE + TILE_SIZE * 0.5f;
		const float draw_y = (float)current.y * TILE_SIZE + TILE_SIZE * 0.5f;
		
		spriteCache.draw_tinted_rotated_sprite_region(*sprite, get_indexed_color(current.z), half_width, half_height, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height, 0.0f);
		
		spriteCache.draw_tinted_rotated_sprite_region(*sprite, WHITE, half_width, half_height, 0.0f, src_height, src_width, src_height, draw_x, draw_y, draw_width, draw_height, 0.0f);
		
	}
	
}*/


void GameManager::draw_ants()
{
	
	std::vector<Ant*> antsToDraw;
	
	for (Ant & c_ant : ant) 
	{
		if (c_ant.active)
			antsToDraw.push_back(&c_ant);
	}
	
	if (!antsToDraw.empty() )
	{
		
		// marcador de selecao
		for (const Ant * c_ant : selectedAnt) 
		{
			if (c_ant->active)
			{
			    
			    const float src_width = selectionCircleSprite.get_width();
				const float src_height = selectionCircleSprite.get_height();
				
				const float draw_width = src_width;
				const float draw_height = src_height;
				
				const float draw_x = c_ant->position.x - src_width * 0.5f;
				const float draw_y = c_ant->position.y - src_height * 0.5f;
				
                spriteCache.draw_sprite_region(selectionCircleSprite, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
			}
		}
		
		// sombras
		for (const Ant * c_ant : antsToDraw)
		{
			if (c_ant->position_z > 0.0f)
				c_ant->draw_shadow();
		}
		
		// formiga em si
		for (const Ant * c_ant : antsToDraw) 
			c_ant->draw();
		
		// comida carregada
		for (const Ant * c_ant : antsToDraw) 
			c_ant->draw_food();
		
		spriteCache.flush();
		
		// hp
		//spriteCache.set_texture(whiteSquareBitmap);
		for (const Ant * c_ant : antsToDraw) 
		{
			if (c_ant->HP > 0 && !c_ant->HP_is_full() )
			{
				//const float factor = (float)c_ant->HP / 10.0f;
				//const float width = TILE_SIZE * factor;
				
				//spriteCache.draw_filled_rectangle(c_ant->position.x - TILE_SIZE * 0.5f, c_ant->position.y - TILE_SIZE * 0.5f, width, 4, get_health_tint(factor) );
				
				const float draw_x = c_ant->position.x; // - TILE_SIZE * 0.5f;
				const float draw_y = c_ant->position.y - TILE_SIZE * 0.5f;
				
				//spriteCache.draw_tinted_sprite_region(whiteSquareBitmap, get_health_tint(factor), 0.0f, 0.0f, whiteSquareBitmap.get_width(), whiteSquareBitmap.get_height(), draw_x, draw_y, width, 4);
				
				draw_HP_bar(draw_x, draw_y, c_ant->HP, 10);
			
			}
		}
		
		// control group numbers
		const float text_scale = the_font.get_scale_for_height(16.0f);
		for (const Ant * c_ant : antsToDraw)
		{
			if (c_ant->groupID > 0)
			{
				
				const float draw_x = c_ant->position.x + TILE_SIZE * 0.5f;
				const float draw_y = c_ant->position.y + TILE_SIZE * 0.5f;
				
				String str = to_stringi( c_ant->groupID < 10 ? c_ant->groupID : 0);
				
				spriteCache.draw_scaled_text_shadow(the_font, str, WHITE, ColorRGBA(0, 0, 0, 0.5f), draw_x, draw_y, text_scale, text_scale, TEXT_ALIGN_RIGHT | TEXT_VERTICAL_ALIGN_ABOVE_BASELINE);
				
			}
			
		}
		
	}
	
	
}



void GameManager::draw_food()
{
	
	for (const Food & entity : food) 
	{
		if (entity.active)
			entity.draw();
	}
	
	for (const FoodChunk & entity : foodChunk) 
	{
		if (entity.active)
			entity.draw();
	}
	
}



void GameManager::draw_interface()
{
	
	const float screen_width = screen_w;
	const float screen_height = screen_h;
	float font_size = 32.0f;
	
	const bool mouse_over_UI_ = mouse_over_UI();
	
	if (editMode)
	{
		
		
		std::string name_str;
		int variant_count = 0;
		
		switch (editorElementType)
		{
			
			case EDITOR_ELEMENT::TERRAIN:
			{
				
				switch (editorTileType)
				{
					
					case TILE::DIRT: name_str = "Dirt"; break;
					case TILE::SAND: name_str = "Sand"; break;
					case TILE::ROCKY_GROUND: name_str = "Rocky Ground"; break;
					case TILE::MUD: name_str = "Mud"; break;
					case TILE::WATER: name_str = "Water"; break;
					case TILE::_VOID_: name_str = "Void"; break;
					default: name_str = "???"; 
					
				}
				break;
				
			}
			
			case EDITOR_ELEMENT::TERRAIN_OBJECT: 
			{
				switch (editorTileType)
				{
					
					case SECONDARY_TILE::FIRE: name_str = "Fire"; break;
					case SECONDARY_TILE::MUD_BRIDGE: name_str = "Mud bridge"; break;
					case SECONDARY_TILE::LANDMINE_0: name_str = "Landmine"; break;
					default: name_str = "???"; 
					
				}
				break;
			}
			
			case EDITOR_ELEMENT::ANTHILL: 
			{
				name_str = "Ant Hill"; 
				break;
			}
			
			case EDITOR_ELEMENT::ANT: 
			{
				
				switch (editorTileType)
				{
					
					case POWERUP::STRENGTH: name_str = "Combat Ant"; break;
					case POWERUP::FIREMAN: name_str = "Fire Ant"; break;
					case POWERUP::ROBBER: name_str = "Thief Ant"; break;
					case POWERUP::DIVER: name_str = "Swimer Ant"; break;
					case POWERUP::BOMBER: name_str = "Bomber Ant"; break;
					default: name_str = "Ant"; 
					
				}
				break;
				
			}
			
			case EDITOR_ELEMENT::POWERUP:
			{
				
				switch (editorTileType)
				{
					
					case POWERUP::STRENGTH: name_str = "Combat Powerup"; break;
					case POWERUP::FIREMAN: name_str = "Fire Powerup"; break;
					case POWERUP::ROBBER: name_str = "Thief Powerup"; break;
					case POWERUP::DIVER: name_str = "Swimer Powerup"; break;
					case POWERUP::BOMBER: name_str = "Bomber Powerup"; break;
					default: name_str = "??? Powerup"; 
					
				}
				break;
				
			}
			
			case EDITOR_ELEMENT::FOOD: 
			{
				
				if (FoodData * data = get_food_data(editorTileType) )
				{
					name_str = data->name; 
					variant_count = data->variants;
					
				}
				break;
				
			}
			
			case EDITOR_ELEMENT::OBSTACLE:
			{
				if (ObstacleData * data = get_obstacle_data(editorTileType) )
				{
					name_str = data->name; 
					variant_count = data->variants;
				}
				break;
			}
			
			case EDITOR_ELEMENT::POWERUP_GENERATOR:
			{
				name_str = "Powerup Generator";
				break;
			}
			
			default: name_str = "Unknown";
			
		}
		
		std::string str = name_str;
		
		// selecao de variant
		if (variant_count > 1)
		{
			
			str += " ";
			int variant_id = (editorVariant>= 0 && editorVariant <= variant_count) ? editorVariant : 0;
			for (int i = 0; i <= variant_count; i++)
			{
				
				if (variant_id == i)
					str += "[" + to_stringi(i) + "]";
				else
					str += to_stringi(i);
				
				str += " ";
				
			}
		}
		
		spriteCache.draw_text_height(the_font, str, WHITE, 10.0f, screen_h - 10, font_size, TEXT_ALIGN_LEFT | TEXT_VERTICAL_ALIGN_BASELINE);
		
		
	}
	
	else // jogo
	{
		
		if (gameEnded)
		{
			/*
			if (gameTime <= 0)
				spriteCache.draw_text_height(the_font, "Time is up!", WHITE, screen_width * 0.5f, screen_height * 0.2f, font_size * 3.0f, TEXT_ALIGN_CENTER);
			*/
			
			// end_game_window
			/*
			// placar de final de jogo
			int player_count = get_player_count(); //player.size();
			if (player_count > 1)
			{
				
				float cursor_x = screen_width * 0.3f;
				float cursor_y = screen_height * 0.35f;
				const float offset = 40.0f * 2.0f;
				
				for (Player & current : player)
				{
					
					std::string str = current.name + ": " + to_stringi(current.foodPoints);
					spriteCache.draw_text_height(the_font, str, current.color, cursor_x, cursor_y, font_size * 2.0f, TEXT_ALIGN_LEFT);
					cursor_y += offset;
					
				}
				
			}*/
			
		}
		
		else
		{
			/*
			if (localPlayer)
			{
				
				//spriteCache.draw_9slice(window9sliceSprite, WHITE, base_x, base_y, width, height, 1.0f);
				
				std::string score_text = "Score: " + to_stringi(localPlayer->foodPoints);
				spriteCache.draw_text_height(the_font, score_text, WHITE, 10.0f, 10.0f, font_size, TEXT_ALIGN_LEFT);
				
			}*/
			
			float font_scale = the_font.get_scale_for_height(font_size);
			
			
			if (!mouse_over_UI_)
			{
				
				// examinar comida
				if (!editMode)
				{
					
					Vector2 pos = camera.mouse_coordinates_to_world_coordinates();
					//if (Food * targetFood = get_food_at_radius(pos.x, pos.y, 1) )
					if (Food * targetFood = get_food_at_point(pos.x, pos.y) )
					{
						
						const int points_per_piece = targetFood->get_points_per_piece();
						const int remaining_bites = targetFood->points;
						const int total_bites = targetFood->get_total_pieces();
						
						String str1 = to_stringi(points_per_piece) + " Pts";
						String str2 = to_stringi(remaining_bites) + " / " + to_stringi(total_bites);
						
						const float w = 100.0f + (total_bites >= 100 ? 20 : 0);
						const float h = font_size * 2;
						
						// posicao da caixa
						Vector2 draw_pos = camera.world_coordinates_to_screen_coordinates(targetFood->get_center_horizontal(), targetFood->get_center_vertical() );
						const float offset = targetFood->get_width() * 0.5f * camera.zoom;
						draw_pos.x += offset;
						
						if (draw_pos.x + w >= screen_w)
							draw_pos.x = screen_w - w; //-= w;
							
						if (draw_pos.y + h >= screen_h)
							draw_pos.y = screen_h - h; //-= h;
						
						// fundo 
						const float a = 0.75f;
						spriteCache.draw_9slice(outlinedSquareBitmap, ColorRGBA(0.5f * a, 0.5f * a, 0.8f * a, a), draw_pos.x, draw_pos.y, w, h, 2.0f, 4, 4);
						
						// texto
						spriteCache.draw_scaled_text_shadow(the_font, str1, WHITE, BLACK, draw_pos.x + 4, draw_pos.y, font_scale, font_scale, TEXT_ALIGN_LEFT, 2);
						spriteCache.draw_scaled_text_shadow(the_font, str2, WHITE, BLACK, draw_pos.x + 4, draw_pos.y + 32, font_scale, font_scale, TEXT_ALIGN_LEFT, 2);
						
					}
				
				}
			
			}
			
			// timer
			std::string time_text = frames_to_time_format_mmss(gameTime);
			spriteCache.draw_scaled_text_shadow(the_font, time_text, WHITE, BLACK, screen_width * 0.5f, 10.0f, font_scale, font_scale, TEXT_ALIGN_CENTER, 2);
			
			// contador de formigas selecionadas
			const int selectedAnt_count = selectedAnt.size();
			if (selectedAnt_count > 0)
			{
				std::string str = to_stringi(selectedAnt_count) + " ants selected";
				spriteCache.draw_text_height(the_font, str, WHITE, 10.0f, screen_height - 40.0f, font_size, TEXT_ALIGN_LEFT);
			}
			
			draw_players_score_box();
		
		}
		
	}
	
	draw_minimap(screen_w - (gameMap.width * 4) - 10, 10, true);
	
	if (console.active)
		console.draw();
	
}



void GameManager::draw_HP_bar(float x, float y, int value, int max_value)
{
	
	const float draw_scale = glm::clamp(1.0f / camera.zoom, 0.5f, 2.0f); //1.0f; //1.0f / camera.zoom; //1.0f; //2.0f;
	
	const float bitmap_width = HPBarSprite.get_width();
	const float bitmap_height = HPBarSprite.get_height() / 2;
	
	const float draw_width = 32.0f; //bitmap_width * draw_scale; 
	const float draw_height = 8.0f; //bitmap_height * draw_scale;
	
	x -= draw_width * 0.5f; // centraliza
	y -= draw_height;
	
	spriteCache.draw_sprite_region(HPBarSprite, 0, 0, bitmap_width, bitmap_height, x, y, draw_width, draw_height);
	
	// parte colorida
	const float fill_ratio = (float)value / (float)max_value;
	if (fill_ratio > 0.0f)
	{
	
		//const float final_width = bar_width * fill_ratio;
		
		const float offset = 0.0f; //2.0f; //4.0f;
		
		const float src_width = (bitmap_width - offset * 2.0f) * fill_ratio;
		const float draw_width2 = (draw_width - (offset * draw_scale * 2.0f) ) * fill_ratio;
		
		spriteCache.draw_tinted_sprite_region(HPBarSprite, get_health_tint(fill_ratio), offset, bitmap_height, src_width, bitmap_height, x + offset * draw_scale, y, draw_width2, draw_height);
		
	}
	
	// ordem invertida pra que o brilho caia encima da barra cheia
	// fundo fixo
	//spriteCache.draw_sprite_region(HPBarSprite, 0, 0, bitmap_width, bitmap_height, x, y, draw_width, draw_height);
	//fd_draw_3slice(fd_cache, slider_bar_sprite, WHITE, x, y, bar_width, bar_height);
	
	//std::string text = std::to_string(value) + "/" + std::to_string(max_value);
	//fd_text_cache.draw_scaled_text_shadow(the_font, text, WHITE, BLACK, x + draw_width / 2, y + (draw_height - 32) / 2, 1.0f, 1.0f, TEXT_ALIGN_CENTER, 2);
	
	
}



void GameManager::draw_minimap(float x, float y, bool draw_background)
{
	
	const float tileSize = 4;
	
	if (draw_background)
	{
		
		const float offset = 10.0f;
		
		spriteCache.draw_9slice(window9sliceSprite, WHITE, x - offset, y - offset, (float)(gameMap.width * tileSize) + offset * 2.0f, (float)(gameMap.height * tileSize) + offset * 2.0f, 2.0f, 4, 4);
		
	}
	
	
	Bitmap & texture = whiteSquareBitmap;
	const float src_width = texture.get_height();
	
	// terreno
	for (int y1 = 0; y1 < gameMap.height; y1++)
	{
		
		for (int x1 = 0; x1 < gameMap.width; x1++)
		{
			
			ColorRGBA tint;
			
			if (gameMap.terrain[y1][x1].solid)
				tint = color_int_to_float(35, 91, 51, 255);
				
			else
			{
				
				int type = gameMap.terrain[y1][x1].type;
				
				
				switch (type)
				{
					
					case TILE:: DIRT: tint = color_int_to_float(183, 170, 120); break;
					case TILE:: SAND: tint = color_int_to_float(214, 207, 178); break;
					case TILE:: ROCKY_GROUND: tint = color_int_to_float(139, 139, 151); break;
					case TILE:: MUD: tint = color_int_to_float(106, 97, 88); break;
					case TILE:: WATER: tint = color_int_to_float(16, 56, 136);  break;
					
					default: tint = ColorRGBA(0, 0, 0, 1);
					
				}
			
			}
			
			spriteCache.draw_tinted_sprite_region( texture, tint, 0.0f, 0.0f, src_width, src_width, x + x1 * tileSize, y + y1 * tileSize, tileSize, tileSize);
			
		}
		
	}
	
	
	
	// ---- objetos
	const float pos_multiplier = tileSize / TILE_SIZE;
	
	const ColorRGBA item_color = color_int_to_float(240, 94, 146);
	const ColorRGBA food_color = color_int_to_float(246, 148, 6);
	
	Bitmap & anthillTexture = minimapAntHillSprite;
	const float anthill_src_width = anthillTexture.get_width();
	const float anthill_dst_width = anthill_src_width;
	
	Bitmap & antTexture = minimapAntSprite;
	const float ant_src_width = antTexture.get_width();
	const float ant_dst_width = ant_src_width;
	
	if (gameInProgress)
	{
		
		// powerups
		for (const Powerup & currentPowerup : powerup) 
		{
			if (currentPowerup.active)
			{
				
				const IntVector2 tilePosition = currentPowerup.get_tile_position();
				const float draw_x = x + (float)tilePosition.x * tileSize;
				const float draw_y = y + (float)tilePosition.y * tileSize;
				
				spriteCache.draw_tinted_sprite_region( texture, item_color, 0.0f, 0.0f, src_width, src_width, draw_x, draw_y, tileSize, tileSize);
				
			}
		}
		
		// food
		for (const Food & currentFood : food) 
		{
			if (currentFood.active)
			{
				
				/*
				const float width = currentFood.get_tile_width();
				const IntVector2 tilePosition = currentFood.get_tile_position();
				const float draw_x = x + (float)tilePosition.x * tileSize - width * 0.5f;
				const float draw_y = y + (float)tilePosition.y * tileSize - width * 0.5f;
				*/
				
				const float width = currentFood.get_width() * pos_multiplier;
				const float height = currentFood.get_height() * pos_multiplier;
				//const float draw_x = x + currentFood.position.x * pos_multiplier - width * 0.5f;
				//const float draw_y = y + currentFood.position.y * pos_multiplier - width * 0.5f;
				const float draw_x = x + currentFood.get_left() * pos_multiplier;
				const float draw_y = y + currentFood.get_top() * pos_multiplier;
				
				spriteCache.draw_tinted_sprite_region( texture, food_color, 0.0f, 0.0f, src_width, src_width, draw_x, draw_y, width, height);
				
			}
		}
		
		
		// anthills
		for (AntHill & currenAnthill : antHill)
		{
			if (currenAnthill.active)
			{
				
				const bool symetric = is_bit_flag_set(currenAnthill.flags, ANTHILL_SYMMETRICAL);
				
				const float draw_x = x + currenAnthill.position.x * pos_multiplier + (symetric ? - anthill_dst_width * 0.5f : 0.0f);
				const float draw_y = y + currenAnthill.position.y * pos_multiplier + (symetric ? - anthill_dst_width * 0.5f : 0.0f);
				
				spriteCache.draw_tinted_sprite_region( anthillTexture, currenAnthill.get_my_Color(), 0.0f, 0.0f, anthill_src_width, anthill_src_width, draw_x, draw_y, anthill_dst_width, anthill_dst_width);
			}
		}
		
		// ants
		for (Ant & currenAnt : ant)
		{
			if (currenAnt.active)
			{
				
				// posicao baseada em tiles
				//const IntVector2 tilePosition = currenAnt.get_tile_position();
				//const float draw_x = x + (float)tilePosition.x * tileSize + tileSize * 0.5f - ant_dst_width * 0.5f;
				//const float draw_y = y + (float)tilePosition.y * tileSize + tileSize * 0.5f - ant_dst_width * 0.5f;
				
				// posicao float
				const float draw_x = x + currenAnt.position.x * pos_multiplier - ant_dst_width * 0.5f;
				const float draw_y = y + currenAnt.position.y * pos_multiplier - ant_dst_width * 0.5f;
				
				spriteCache.draw_tinted_sprite_region( antTexture, currenAnt.get_my_bright_Color(), 0.0f, 0.0f, ant_src_width, ant_src_width, draw_x, draw_y, ant_dst_width, ant_dst_width);
			}
		}
		
	}
	
	// --------- spawners do mapa
	else 
	{
		
		const int player_size = player.size();
		
		// powerups
		for (const IntVector3 & currentPowerup : gameMap.itemSpawn) 
		{

			const float draw_x = x + (float)currentPowerup.x * tileSize;
			const float draw_y = y + (float)currentPowerup.y * tileSize;
			
			spriteCache.draw_tinted_sprite_region( texture, item_color, 0.0f, 0.0f, src_width, src_width, draw_x, draw_y, tileSize, tileSize);
			
		}
		
		// food
		for (const IntVector3 & currentFood : gameMap.foodSpawn) 
		{
			
			FoodData * currentData = get_food_data(currentFood.z);
			
			const float width = currentData ? currentData->radius * 2.0f * pos_multiplier : 1.0f;
			
			const float draw_x = x + currentFood.x * tileSize; //tileSize - width * 0.5f;
			const float draw_y = y + currentFood.y * tileSize; //tileSize - width * 0.5f;
			
			spriteCache.draw_tinted_sprite_region( texture, food_color, 0.0f, 0.0f, src_width, src_width, draw_x, draw_y, width, width);
			
		}
		
		// anthills
		for (IntVector4 & currenAnthill : gameMap.antHillSpawn)
		{
			
			const bool symetric = is_bit_flag_set(currenAnthill.w, ANTHILL_SYMMETRICAL);
			
			const float draw_x = x + currenAnthill.x * tileSize + (symetric ? - anthill_dst_width * 0.5f : 0.0f);
			const float draw_y = y + currenAnthill.y * tileSize + (symetric ? - anthill_dst_width * 0.5f : 0.0f);
			
			ColorRGBA tint; //get_indexed_color(currenAnthill.z)
			const int player_index = currenAnthill.z - 1;
			if (player_index >= 0 && player_index < player_size && player[player_index].active)
				tint = player[player_index].get_bright_color();
			else
				tint = ColorRGBA(0.5f, 0.5f, 0.5f, 0.5f);
			
			spriteCache.draw_tinted_sprite_region( anthillTexture, tint, 0.0f, 0.0f, anthill_src_width, anthill_src_width, draw_x, draw_y, anthill_dst_width, anthill_dst_width);
			
		}
		
		// ants
		for (IntVector3 & currenAnt : gameMap.antSpawn)
		{

			const float draw_x = x + currenAnt.x * tileSize - ant_dst_width * 0.5f;
			const float draw_y = y + currenAnt.y * tileSize - ant_dst_width * 0.5f;
			
			ColorRGBA tint; //get_indexed_color(currenAnthill.z)
			const int player_index = currenAnt.z - 1;
			if (player_index >= 0 && player_index < player_size && player[player_index].active)
				tint = player[player_index].get_bright_color();
			else
				tint = ColorRGBA(0.5f, 0.5f, 0.5f, 0.5f);
			
			spriteCache.draw_tinted_sprite_region( antTexture, tint, 0.0f, 0.0f, ant_src_width, ant_src_width, draw_x, draw_y, ant_dst_width, ant_dst_width);
			
		}
		
		
	}
	
}



void GameManager::draw_players_score_box()
{
	
	// TODO: otimizar pre calculando a escala da font
	
	int player_count = get_player_count(); //player.size();
	//if (player_count > 0)
	{
		
		// encontra o nome mais longo
		float longest_name = 100.0f;
		for (Player & current : player)
		{
			if (current.active)
			{
				
				const float name_width = the_font.get_text_width_for_height(current.name, 32);
				if (name_width > longest_name)
					longest_name = name_width;
			}
			
		}
		
		float font_size = 32.0f;
		
		const float border = 10.0f;
		
		const float points_str_space = the_font.get_text_width_for_height(": 000000", 32);
		
		const float text_hor_space = longest_name + points_str_space; //200.0f;
		const float text_spacing = 2.0f;
		const float text_border_distance = 10.0f;
		
		const float inside_width = text_hor_space + text_border_distance * 2.0f;
		const float inside_height = (float)player_count * font_size + (float)(player_count - 1) * text_spacing + text_border_distance * 2.0f;
		
		const float total_width = inside_width + border * 2.0f;
		const float total_height = inside_height + border * 2.0f;
		
		const float base_x = 0.0f; //screen_width - width - 10.0f;
		const float base_y = 0.0f; //screen_height - height - 10.0f;
		
		spriteCache.draw_9slice(window9sliceSprite, WHITE, base_x, base_y, total_width, total_height, 2.0f, 4, 4);
		spriteCache.draw_9slice(window9sliceInsideSprite, WHITE, base_x + border, base_y + border, inside_width, inside_height, 2.0f, 4, 4);
		
		float cursor_x = base_x + border + text_border_distance;
		float cursor_y = base_y + border + text_border_distance;
		
		const float line_offset = font_size + text_spacing;
		const float half_x = cursor_x + longest_name; //inside_width * 0.5f;
		
		for (Player & currentPlayer : player)
		{
			if (currentPlayer.active)
			{
				//std::string str = current.name + ": " + to_stringi(current.foodPoints);
				//spriteCache.draw_text_height(the_font, str, current.color, cursor_x, cursor_y, font_size, TEXT_ALIGN_LEFT | TEXT_VERTICAL_ALIGN_BELLOW_BASELINE);
				
				String str1 = currentPlayer.name;
				String str2 = ": " + to_stringi(currentPlayer.foodPoints);
				
				spriteCache.draw_text_height(the_font, str1, currentPlayer.get_color(), half_x, cursor_y, font_size, TEXT_ALIGN_RIGHT | TEXT_VERTICAL_ALIGN_BELLOW_BASELINE);
				spriteCache.draw_text_height(the_font, str2, 		 WHITE, half_x, cursor_y, font_size, TEXT_ALIGN_LEFT | TEXT_VERTICAL_ALIGN_BELLOW_BASELINE);
				
				cursor_y += line_offset;
			}
		}
		
	}
	
}
