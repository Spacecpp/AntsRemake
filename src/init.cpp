#include "init.h"
#include "Alegrinho.h"
#include "Alegrinho_GUI.h"
#include "globals.h"
#include <stdio.h>
#include "lists.h"
#include "Powerup.h"

bool program_init()
{
	
	if (!init_main_module() ) return false;
    if (!init_font_module() ) return false;
    if (!init_audio_module() ) return false;
    if (!init_network_module() ) return false;
    
    if (!create_main_window(DEFAULT_SCREEN_W, DEFAULT_SCREEN_H, "Ants") )
	{
		shutdown_everything();
		return false;
	}
	
	global_textureFilterMode = TEXTURE_FILTER_NEAREST;
	
    if (!data_init() )
	{
		printf("Failed to load data\n");
		//shutdown_everything();
		program_shutdown();
		return false;
	}
	
	return true;
	
}



bool data_init()
{
    
    #ifndef EMSCRIPTEN
	set_window_icon("Data/Graphics/icon.png");
	#endif
	
	// shaders
	if (!init_default_shader(spriteShader) ) return false;
	if (!init_default_point_shader(pointShader) ) return false;
	
	// cursor
	if (customCursor.load("Data/Graphics/cursor_default.png") )
		customCursor.use();
		
	// bitmaps
	if (!create_single_color_texture(whiteSquareBitmap, WHITE, 32, 32) ) return false;
	if (!create_outlined_rectangle_texture(outlinedSquareBitmap, ColorRGBA(1.0f, 1.0f, 1.0f, 1.0f), ColorRGBA(0.75f, 0.75f, 0.75f, 1.0f) , 32, 32) ) return false;
	
	
	// ants
	if (!antSprite.load("Data/Graphics/Ants/ant.png") ) return false;
	if (!antSoldierSprite.load("Data/Graphics/Ants/antSoldier.png") ) return false;
	if (!antFiremanSprite.load("Data/Graphics/Ants/antFireman.png") ) return false;
	if (!antRobberSprite.load("Data/Graphics/Ants/antRobber.png") ) return false;
	if (!antDiverSprite.load("Data/Graphics/Ants/antDiver.png") ) return false;
	if (!antBomberSprite.load("Data/Graphics/Ants/antBomber.png") ) return false;
	
	const int bmpw = 32;
	antFirstFrameSprite[0].create_sub(antSprite, 0, 0, bmpw, bmpw);
	antFirstFrameSprite[1].create_sub(antSoldierSprite, 0, 0, bmpw, bmpw);
	antFirstFrameSprite[2].create_sub(antFiremanSprite, 0, 0, bmpw, bmpw);
	antFirstFrameSprite[3].create_sub(antRobberSprite, 0, 0, bmpw, bmpw);
	antFirstFrameSprite[4].create_sub(antDiverSprite, 0, 0, bmpw, bmpw);
	antFirstFrameSprite[5].create_sub(antBomberSprite, 0, 0, bmpw, bmpw);
	
	//if (!antHillDefaultSprite.load("Data/Graphics/anthill_default.png") ) return false;
	for (int i = 0; i < 4; i++)
	{
		String filename = "hill_body_" + to_stringi(i) + ".png";
		if (!antHillDefaultSprite[i].load("Data/Graphics/Anthills/Default/" + filename) ) return false;
	}
	
	if (!antHillSymetricSprite.load("Data/Graphics/anthill_symetric.png") ) return false;
	if (!antHillFlagSprite.load("Data/Graphics/anthill_flag.png") ) return false;
	
	// terrain
	if (!waterTexture.load("Data/Graphics/Terrain/water.png") ) return false;
	if (!dirtTexture.load("Data/Graphics/Terrain/dirt.png") ) return false;
	if (!sandTexture.load("Data/Graphics/Terrain/sand.png") ) return false;
	if (!rockyGroundTexture.load("Data/Graphics/Terrain/rocky_ground.png") ) return false;
	if (!mudTexture.load("Data/Graphics/Terrain/mud.png") ) return false;
	
	if (!mudTransitionTileSprite.load("Data/Graphics/Terrain/mud_transition.png") ) return false;
	if (!mudToWaterTransitionTileSprite.load("Data/Graphics/Terrain/mud_water_transition.png") ) return false;
	if (!sandTransitionTileSprite.load("Data/Graphics/Terrain/sand_transition.png") ) return false;
	if (!rockyTransitionTileSprite.load("Data/Graphics/Terrain/rocky_transition.png") ) return false;
	
	if (!fireSprite.load("Data/Graphics/fire.png") ) return false;
	if (!landMineSprite.load("Data/Graphics/land_mine.png") ) return false;
	if (!explosionSprite.load("Data/Graphics/explosion.png") ) return false;
	if (!mudBridgeSprite.load("Data/Graphics/mud_bridge.png") ) return false;
	if (!create_single_color_texture(voidTileTexture, BLACK, TILE_SIZE, TILE_SIZE) ) return false;
	
	if (!foodPieceSprite.load("Data/Graphics/food_piece.png") ) return false;
	//SetTextureWrap(foodPieceSprite, TEXTURE_WRAP_CLAMP);
	
	if (!powerupAtlas.load("Data/Graphics/powerups.png") ) return false;
	const float powerup_width = powerupAtlas.get_height();
	for (int i = 0; i < POWERUP::COUNT - 1; i++)
	{
		powerupSprite[i].create_sub(powerupAtlas, i * powerup_width, 0, powerup_width, powerup_width);
	}
	
	if (!powerupDropSprite.load("Data/Graphics/powerup_drop.png") ) return false;
	
	
	if (!missingSprite.load("Data/Graphics/missing_texture.png") ) return false;
	if (!translucentSquaresSprite.load("Data/Graphics/translucent_squares.png") ) return false;
	if (!selectionCircleSprite.load("Data/Graphics/selection_circle.png") ) return false;
	
	if (!menuBackgroundTexture.load("Data/Graphics/background.png") ) return false;
	
	if (!HPBarSprite.load("Data/Graphics/hp_bar.png") ) return false;
	
	if (!minimapAntSprite.load("Data/Graphics/minimap_ant.png") ) return false;
	if (!minimapAntHillSprite.load("Data/Graphics/minimap_anthill.png") ) return false;
	
	if (!healParticleSprite.load("Data/Graphics/heal_particle.png") ) return false;
	
	if (!deathSmokeSprite.load("Data/Graphics/death_smoke.png") ) return false;
	if (!AntTransformationSprite.load("Data/Graphics/transformation.png") ) return false;
	
	if (!readyCheckmarkSprite.load("Data/Graphics/ready_checkmark.png") ) return false;
	if (!logoSprite.load("Data/Graphics/logo.png") ) return false;
	
	
	// GUI
	if (!buttonSprite.load("Data/Graphics/GUI/button.png") ) return false;
	if (!checkBoxSprite.load("Data/Graphics/GUI/checkbox.png") ) return false;
	if (!window9sliceSprite.load("Data/Graphics/GUI/window9slice.png") ) return false;
	if (!window9sliceInsideSprite.load("Data/Graphics/GUI/window9slice_inside.png") ) return false;
	if (!console9sliceSprite.load("Data/Graphics/GUI/console9slice.png") ) return false;
	if (!arrowUpSprite.load("Data/Graphics/GUI/arrow_up.png") ) return false;
	if (!arrowDownSprite.load("Data/Graphics/GUI/arrow_down.png") ) return false;
	if (!scrollBoxSprite.load("Data/Graphics/GUI/scroll_box.png") ) return false;
	if (!sliderBarBitmap.load("Data/Graphics/GUI/slider_bar.png") ) return false;
	if (!sliderCursorBitmap.load("Data/Graphics/GUI/slider_cursor.png") ) return false;
	if (!colorChooserSprite.load("Data/Graphics/GUI/color_chooser.png") ) return false;
	
	if (!neutralFrameSprite.load("Data/Graphics/GUI/neutral_frame.png") ) return false;
	
	if (!editorIconAtlas.load("Data/Graphics/GUI/editor_icons.png") ) return false;
	const float editor_icon_width = editorIconAtlas.get_height();
	for (int i = 0; i < EDITOR_ICON_SPRITE_COUNT; i++)
	{
		editorIconSprite[i].create_sub(editorIconAtlas, i * editor_icon_width, 0, editor_icon_width, editor_icon_width);
	}
	
	if (!helpScreenImage.load("Data/Graphics/help_screen.png") ) return false;
	
	
	std::string font_path = "Data/Gamer.ttf";
	//std::string font_path = "Data/Gamer_0.ttf";
	//std::string font_path = "Data/NotoSans-Medium.ttf";
	
	if (!the_font.load(font_path.c_str(), 72) ) return false;
	
	// --- Sons
	if (!s_click.load("Data/Sounds/click.wav") ) return false;
	if (!s_error.load("Data/Sounds/error.wav") ) return false;
	if (!s_chat.load("Data/Sounds/chat.wav") ) return false;
	if (!s_connect.load("Data/Sounds/connect.wav") ) return false;
	if (!s_player_left.load("Data/Sounds/player_left.wav") ) return false;
	
	if (!s_bite.load("Data/Sounds/bite.wav") ) return false;
	if (!s_punch.load("Data/Sounds/punch.wav") ) return false;
	if (!s_point.load("Data/Sounds/point.wav") ) return false;
	if (!s_hatch.load("Data/Sounds/hatch.wav") ) return false;
	if (!s_newAnt.load("Data/Sounds/new_ant.wav") ) return false;
	if (!s_get_powerup.load("Data/Sounds/get_powerup.wav") ) return false;
	if (!s_splash.load("Data/Sounds/splash.wav") ) return false;
	if (!s_drown.load("Data/Sounds/drown.wav") ) return false;
	if (!s_powerupDrop.load("Data/Sounds/powerup_drop.wav") ) return false;
	if (!s_create_fire.load("Data/Sounds/create_fire.wav") ) return false;
	if (!s_extinguish_fire.load("Data/Sounds/extinguish_fire.wav") ) return false;
	if (!s_steal.load("Data/Sounds/steal.wav") ) return false;
	if (!s_lay_mine.load("Data/Sounds/lay_mine.wav") ) return false;
	if (!s_explosion.load("Data/Sounds/explosion.wav") ) return false;
	if (!s_small_explosion.load("Data/Sounds/small_explosion.wav") ) return false;
	if (!s_puff.load("Data/Sounds/puff.wav") ) return false;
	if (!s_thiefAlert.load("Data/Sounds/thief_alert.wav") ) return false;
	if (!s_alarm.load("Data/Sounds/alarm.wav") ) return false;
	
	if (!s_victory.load("Data/Sounds/victory.wav") ) return false;
	if (!s_defeat.load("Data/Sounds/defeat.wav") ) return false;
	
	if (!s_reporting1.load("Data/Sounds/reporting1.wav") ) return false;
	if (!s_moving1.load("Data/Sounds/moving1.wav") ) return false;
	if (!s_attack1.load("Data/Sounds/attack1.wav") ) return false;
	if (!s_no_no.load("Data/Sounds/no_no.wav") ) return false;
	
	// music
	mus_mainMenu.load("Data/Musics/INTRO.ogg", true);
	
	spriteCache.create_sprite_cache();
	spriteCache2.create_sprite_cache();
	textCache.create_sprite_cache();
	
	// GUI pointers
	GUI_default_button_sprite = &buttonSprite;
	GUI_default_checkbox_sprite = &checkBoxSprite;
	GUI_font = &the_font;
	GUI_draw_cache = &spriteCache;
	GUI_white_sprite = &whiteSquareBitmap;
	
	GUI_button_down_sprite = &arrowDownSprite;
	GUI_button_up_sprite = &arrowUpSprite;
	GUI_9slice_sprite = &window9sliceSprite;
	
	GUI_slider_cursor_sprite = &sliderCursorBitmap;
	GUI_slider_bar_sprite = &sliderBarBitmap;
	GUI_slider_bar_fill_sprite = &sliderCursorBitmap;
	
	GUI_default_textbox_sprite = &window9sliceInsideSprite;
	
	GUI_listbox_sprite = &window9sliceInsideSprite;
	
	GUI_click_sound = &s_click;
	GUI_WINDOW_TITLE_FONT_HEIGHT = 32.0f;
	
	GUI_textbox_slice_scale = 2.0f;
	GUI_textbox_slice_width = 2.0f;
	GUI_listbox_slice_scale = 2.0f;
	GUI_listbox_slice_width = 2.0f;
	
	GUI_color_chooser_sprite = &colorChooserSprite;
	
	generate_terrain_atlas();
	
	return true;
	
}



bool generate_terrain_atlas()
{
	
	AtlasGenerator atlas;
	
	char repeat = 1;
	
	atlas.add_texture(dirtTexture, repeat, false);
	atlas.add_texture(sandTexture, repeat, false);
	atlas.add_texture(rockyGroundTexture, repeat, false);
	atlas.add_texture(mudTexture, repeat, false);
	
	atlas.add_texture(mudTransitionTileSprite, false, false);
	atlas.add_texture(mudToWaterTransitionTileSprite, false, false);
	atlas.add_texture(sandTransitionTileSprite, false, false);
	atlas.add_texture(rockyTransitionTileSprite, false, false);
	atlas.add_texture(mudBridgeSprite, false, false);
	atlas.add_texture(voidTileTexture, false, false);
	
	if (!atlas.create(terrainAtlas, spriteShader, pointShader) )
		return false;
	
	if (!atlas.swap_all_bitmaps_for_sub_bitmaps() ) 
		return false;
	
	return true;
	
}



void program_shutdown()
{
    
    // shaders
    spriteShader.destroy();
    pointShader.destroy();
    
    spriteCache.destroy();
    spriteCache2.destroy();
    textCache.destroy();
    
	
	customCursor.destroy();
	
	// bitmaps
	whiteSquareBitmap.destroy();
	outlinedSquareBitmap.destroy();
	
	
	for (int i = 0; i < 6; i++)
		antFirstFrameSprite[i].destroy();
	
	antSprite.destroy();
	antSoldierSprite.destroy();
	antFiremanSprite.destroy();
	antRobberSprite.destroy();
	antDiverSprite.destroy();
	antBomberSprite.destroy();
	
	//antHillDefaultSprite.destroy();
	for (int i = 0; i < 4; i++)
		antHillDefaultSprite[i].destroy();
		
	antHillSymetricSprite.destroy();
	antHillFlagSprite.destroy();
	
	
	waterTexture.destroy();
	dirtTexture.destroy();
	sandTexture.destroy();
	rockyGroundTexture.destroy();
	mudTexture.destroy();
	
	mudTransitionTileSprite.destroy();
	mudToWaterTransitionTileSprite.destroy();
	sandTransitionTileSprite.destroy();
	rockyTransitionTileSprite.destroy();
	
	fireSprite.destroy();
	landMineSprite.destroy();
	explosionSprite.destroy();
	mudBridgeSprite.destroy();
	voidTileTexture.destroy();
	
	terrainAtlas.destroy();
	
	
	foodPieceSprite.destroy();
	
	for (int i = 0; i < POWERUP::COUNT - 1; i++)
		powerupSprite[i].destroy();
	powerupAtlas.destroy();
	
	powerupDropSprite.destroy();
	
	missingSprite.destroy();
	translucentSquaresSprite.destroy();
	selectionCircleSprite.destroy();
	
	menuBackgroundTexture.destroy();
	
	HPBarSprite.destroy();
	
	minimapAntSprite.destroy();
	minimapAntHillSprite.destroy();
	
	healParticleSprite.destroy();
	
	deathSmokeSprite.destroy();
	AntTransformationSprite.destroy();
	
	readyCheckmarkSprite.destroy();
	logoSprite.destroy();
	
	// GUI
	buttonSprite.destroy();
	checkBoxSprite.destroy();
	window9sliceSprite.destroy();
	window9sliceInsideSprite.destroy();
	console9sliceSprite.destroy();
	arrowUpSprite.destroy();
	arrowDownSprite.destroy();
	scrollBoxSprite.destroy();
	sliderBarBitmap.destroy();
	sliderCursorBitmap.destroy();
	colorChooserSprite.destroy();
	
	neutralFrameSprite.destroy();
	
	for (int i = 0; i < EDITOR_ICON_SPRITE_COUNT; i++)
		editorIconSprite[i].destroy();
	editorIconAtlas.destroy();
	
	helpScreenImage.destroy();
	
	// font
	the_font.destroy();
	
	// sounds
	s_click.destroy();
	s_error.destroy();
	s_chat.destroy();
	s_connect.destroy();
	s_player_left.destroy();
	
	s_bite.destroy();
	s_punch.destroy();
	s_point.destroy();
	s_hatch.destroy();
	s_newAnt.destroy();
	s_get_powerup.destroy();
	s_splash.destroy();
	s_drown.destroy();
	s_powerupDrop.destroy();
	s_create_fire.destroy();
	s_extinguish_fire.destroy();
	s_steal.destroy();
	s_lay_mine.destroy();
	s_explosion.destroy();
	s_small_explosion.destroy();
	s_puff.destroy();
	s_thiefAlert.destroy();
	s_alarm.destroy();
	
	s_victory.destroy();
	s_defeat.destroy();
	
	s_reporting1.destroy();
	s_moving1.destroy();
	s_attack1.destroy();
	s_no_no.destroy();
	
	// music
	mus_mainMenu.destroy();
	

	shutdown_everything();
	
}
