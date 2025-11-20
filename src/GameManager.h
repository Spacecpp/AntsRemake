#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H


#include <string>
#include <vector>
#include <deque>
#include <list>

#include "Alegrinho.h"
#include "Alegrinho_GUI.h"

#include "globals.h"
#include "GameMap.h"
#include "Ant.h"
#include "AntHill.h"
#include "Food.h"
#include "FoodChunk.h"
#include "Obstacle.h"
#include "PowerupGenerator.h"
#include "Powerup.h"
#include "Player.h"
#include "FlyingText.h"
//#include "Explosion.h"
#include "AnimatedSprite.h"
#include "FoodData.h"
#include "ObstacleData.h"
#include "GameOrder.h"
#include "Fight.h"
#include "Console.h"
#include "HealParticle.h"

#include "lists.h"


#ifndef EMSCRIPTEN
#define ONLINE_MULTIPLAYER_AVAILABLE 1
#endif


class GameManager
{

	private:
	
	public:
		// objetos relevantes ao gameplay
		std::deque<AntHill> antHill;
		std::deque<Ant> ant;
		std::vector<Food> food;
		std::vector<FoodChunk> foodChunk;
		std::vector<Obstacle> obstacle;
		std::vector<Powerup> powerup;
		std::vector<PowerupGenerator> powerupGenerator;
		std::vector<Player> player;
		std::vector<Fight> fight;
		GameMap gameMap;
		
		std::list<GameOrder> queuedOrder;
		std::list<GameOrder> netQueuedOrder;
		
		// objetos cosmeticos
		std::vector<FlyingText> flyingText;
		//std::vector<Explosion> explosion;
		std::vector<AnimatedSprite> animatedSprite;
		std::vector<HealParticle> healParticle;
		
		
		// ponteiros
		Player * localPlayer;
		std::vector<Ant*> selectedAnt;
		
		
		// visual
		float waterAnimationOffset;
		
		int fireAnimationFrame;
		int fireAnimationDelay;
		
		int8_t mineAnimationFrame;
		int8_t mineAnimationDelay;
		
		// gameplay related
		int16_t antNextUniqueID;
		int8_t playerNextUniqueID;
		uint32_t gameFrame,
				 commandDelay,
				 networkFrame,
				 gameFrameMax;
        bool gameFrameDone;
		int gameTime;
		bool gameInProgress;
		bool gameEnded;
		bool inGameSetupScreen;
		bool pendingResync;
		RNG gameRNG;
		
		// change detection
		uint32_t oldCommandDelay;
		
		// input
		bool draggingBoxSelection;
		Vector4 boxSelection;
		
		Camera2D camera;
		bool mapDragging;
		Vector2 mapDragPosition;
		
		// console
		Console console;
		
		//bool consoleOpen;
		//std::list< ChatString > consoleMessage;
		//std::string consoleInput;
		
		// editor
		bool editMode;
		int editorTileType;
		int editorElementType;
		uint8_t editorVariant;
		int editorPowerupGenTime;
		uint8_t editorAntHillFlags;
		int8_t editorPlayerID;
		int8_t editorPage;
		String editorSaveFilename;
		
		// database
		std::vector<FoodData> foodData;
		std::vector<ObstacleData> obstacleData;
		
		GUI_Window _GUI_Window;
		GUI_Window mapEditor_upperMenu;
		GUI_Window mapEditor_toolWindow;
		
		#ifdef ONLINE_MULTIPLAYER_AVAILABLE
			NetController netController;
		#endif
		
		//int GUI_PressedButton;
		std::string joinIP;
		int port;
		std::string localPlayerName;
		std::string selectedMapName;
		
		// animacao do menu
		int8_t menuAntAnimationFrame;
		int8_t menuAntAnimationTime;
		
		// animacao das bandeiras dos formigueiros
		int8_t flagAnimationFrame;
		int8_t flagAnimationTime;
		
		// debug
		bool debug_showPaths;
		bool simulateConnectionLost;
		
		// options
		bool edgeScrolling;
		
		bool musicDisabled;
		String currentMusicFileName;
		
		
		// -----------------
		GameManager();
		
		void reset();
		void reset_gameplay();
		
		void main_loop();
		void shutdown();
		void do_logic();
		
		
		void enter_main_menu();
		void leave_game();
		
		void escape_command();
		
		void process_inputs();
		void process_gameplay_inputs();
		void process_editor_inputs();
		
		// local player input
		bool input_order_selected_ant_back_to_anthill();
		
		//void read_text_input();
		
		void refresh_camera();
		void compute_mouse_camera_movement(float min_x, float min_y, float max_x, float max_y);
		void center_camera();
		
		void compute_pressed_button(GUI_Button & c_Button);
		
		bool mouse_over_UI();
		
		bool play_random_ingame_music();
		
		// gameplay
		bool game_can_start();
		void attempt_to_start_game();
		void setup_new_game();
		//void start_single_player_game();
		void setup_single_player_game();
		void enter_map_editor();
		//void start_multiplayer_game();
		void setup_multiplayer_game();
		void start_game();
		void update_gameframe();
		void update_gameplay();
		
		Player * add_player(const Player & newPlayer);
		bool remove_player(Player & targetPlayer);
		bool add_bot();
		bool remove_bot();
		
		void GenerateMap();
		bool play_area_sound(Sound & sound, float x, float y);
		bool play_area_sound(Sound & sound, int x, int y);
		
		float get_move_cost(const IntVector2 & a, const IntVector2 & b);
		bool find_path( const IntVector2 & start, const IntVector2 & end, std::queue<IntVector2> & path, int8_t movementType, bool oneBefore, uint8_t playerIndex);
		
		bool remove_object_at_tile(int x, int y);
		
		bool tile_completely_unoccupied(int x, int y);
		bool tile_occupied(int x, int y, const Ant * current = NULL);
		bool tile_occupied_by_object(int x, int y, const Ant * current = NULL);
		//bool tile_is_walkable(int8_t type);
		bool tile_is_walkable(int x, int y, int8_t movementType);
		bool tile_is_walkable(Tile & currentTile, int8_t movementType);
		//bool tile_is_walkable(int x, int y, int8_t movementType, int8_t playerIndex = -1);
		//bool tile_is_walkable(Tile & currentTile, int8_t movementType, int8_t playerIndex = -1);
		//bool coordinates_out_of_bounds(int x, int y);
		//int8_t get_tile_type(int x, int y);
		bool tile_has_landmine(int x, int y, int8_t playerIndex = -1);
		
		void calculate_solid_tiles();
		
		void resolve_collisions();
		
		bool create_powerup_around_tile(int x, int y, int powerupID, bool allow_center);
		
		size_t get_player_count();
		size_t get_ant_count();
		size_t get_powerup_count();
		
		size_t get_next_player_free_index();
		
		
		void set_selected_ants_control_group(int id);
		void select_control_group(int id);
		void select_all_ants();
		
		// console
		void place_console();
		void add_message_to_the_console(const std::string & message, Player * sender = NULL);
		void add_chat_message_to_the_console(const std::string & message, Player * sender);
		
		// get
		Player * find_local_player();
		Ant * get_Ant(int16_t unique_id);
		Ant * get_Ant_at_position(float x, float y);
		std::vector<Ant*> get_player_ants(Player & targetPlayer);
		void get_Ants_by_ids(std::vector<Ant*> & container, std::vector<int16_t> & ids);
		Ant * get_closest_enemy_ant(Ant & referenceAnt);
		Food * get_food_at_tile(int x, int y);
		Food * get_food_at_radius(float x, float y, float radius);
		Food * get_food_at_point(float x, float y);
		Food * get_closest_food(int x, int y);
		Powerup * get_powerup_at_tile(int x, int y);
		Powerup * get_powerup_at_radius(float x, float y, float radius);
		AntHill * get_AntHill(Player & owner);
		AntHill * get_AntHill(Player * owner);
		AntHill * get_AntHill(int player_id);
		AntHill * get_AntHill_at_tile(int x, int y);
		AntHill * get_AntHill_at_radius(float x, float y, float radius);
		Player * get_Player_by_id(int id);
		Player * get_Player_by_connection_id(int connection_id);
		Player * get_Player_by_index(int index);
		
		Vector2 get_mouse_world_position();
		
		FoodData * get_food_data(int food_id);
		ObstacleData * get_obstacle_data(int obstacle_id);
		Bitmap * get_food_sprite(int food_id);
		
		String get_powerup_name(int id);
		
		// draw
		void draw();
		void draw_menu();
		void draw_game_setup_menu();
		void draw_gameplay();
		
		void draw_map();
		
		void draw_gameplay_elements();
		//void draw_editor_elements();
		
		void draw_ants();
		void draw_food();
		
		void draw_interface();
		
		void draw_HP_bar(float x, float y, int value, int max_value);
		
		void draw_minimap(float x, float y, bool draw_background);
		void draw_players_score_box();
		
		// alocacao de objetos
		Ant * create_Ant(const Ant & O);
		FlyingText * create_FlyingText(const FlyingText & O);
		HealParticle * create_HealParticle(const HealParticle & O);
		Powerup * create_Powerup(const Powerup & O);
		//Explosion * create_Explosion(const Explosion & O);
		FoodChunk * create_FoodChunk(const FoodChunk & O);
		
		AnimatedSprite * create_explosion(int x, int y);
		AnimatedSprite * create_explosion(float x, float y);
		AnimatedSprite * create_death_smoke(float x, float y);
		
		
		bool GameObject_inside_rectangle(float x, float y, float width, float height, GameObject & gameObject);
		bool point_on_GameObject(float x, float y, GameObject & gameObject);
		
		bool points_close(float x1, float y1, float x2, float y2, float tolerance);
		bool point_visible_on_camera(float x, float y);
		
		// windows
		void main_menu_window();
		void game_setup_window();
		void create_player_setup_widgets(Player & targetPlayer);
		void remove_player_setup_widgets(Player & targetPlayer);
		void info_window(const std::string & text, bool back_button = false);
		void server_not_responding_window();
		void esc_menu_window();
		void online_window();
		void select_map_window();
		void options_window();
		void end_game_window();
		
		void create_map_editor_upper_menu_window();
		void create_map_editor_tool_window();
		
		void editor_save_map_window();
		void editor_map_properties_window();
		void editor_map_resize_window();
		void confirm_new_map_window();
		
		void game_help_window();
		
		// save load
		bool save_map(const char * filename);
		bool load_map(const char * filename);
		bool load_selected_map();
		
		bool save_config();
		bool load_config();
		
		bool load_food_data();
		bool load_obstacle_data();
		bool load_animation_data();
		
		// AI
		void update_AI_players();
		
		// network
		#ifdef ONLINE_MULTIPLAYER_AVAILABLE
			void Net_process_input();
			void Net_process_output();
			void Server_handle_net_event();
			void Client_handle_net_event();
			
			void Server_handle_new_connection();
			void Server_handle_disconnection_event();
			
			bool start_server();
			bool attempt_to_connect_to_server(std::string ip, int port);
			
			
			// net commands
			void Client_handle_received_packet();
			void Server_handle_received_packet();
			
			void Client_send_join_authentication();
			void Server_comunicate_game_start();
			
			//void send_packet_to_all(BitStream & data, int channel_id, uint32_t packet_flags);
			
			// -- out
			void communicate_player_disconnection(Player & targetPlayer);
			
			void communicate_chat_message(const std::string & message, Player * sender);
			void communicate_map_change();
			void comunicate_rule_update();
			void communicate_update();
			void comunicate_color_change(Player & targetPlayer);
			void comunicate_ready_tick(Player & targetPlayer);
			
			/*
			void communicate_ant_move( std::vector<Ant*> & targetAnts, int x, int y, bool goingAfterFood);
			void communicate_ant_attack( std::vector<Ant*> & targetAnts, Ant & defenderAnt);
			void communicate_ant_ability_use(Ant & targetAnt, int x, int y, int8_t option);
			void communicate_egg_hatch(AntHill & targetAntHill);
			*/
			
			// -- in
			void read_chat_message_packet(BitStream & _data);
			void read_update_packet(BitStream & _data, Player * senderPlayer);
			
			/*
			void read_ant_move_packet(BitStream & _data);
			void read_ant_attack_packet(BitStream & _data);
			void read_ant_ability_use_packet(BitStream & _data);
			void read_egg_hatch_packet(BitStream & _data);
			*/
		#endif
		
		
		inline bool is_online()
		{
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
			return netController.is_online();
			#else
			return false;
			#endif
		}
		
		inline bool is_offline() { return !is_online(); }
		
		inline bool is_client()
		{
			#ifdef ONLINE_MULTIPLAYER_AVAILABLE
			return netController.is_client();
			#else
			return false;
			#endif
		}
		
		
		// colors
		//ColorRGBA get_indexed_color(int index);
		ColorRGBA get_health_tint(float factor);
		
		// orders
		void update_order_queue();
		void execute_order(GameOrder & order);
		
		bool order_ant_move(bool apply_now, std::vector<Ant*> & targetAnts, int x, int y, bool goingAfterFood);
		bool order_ant_attack(bool apply_now, std::vector<Ant*> & targetAnts, Ant & defenderAnt);
		bool order_ant_use_special_ability(bool apply_now, Ant & targetAnt, int x, int y, int8_t option);
		bool order_anthill_hatch_egg(bool apply_now, AntHill & targetAntHill);
		
		void pack_gamestate(BitStream & data);
		void unpack_gamestate(BitStream & _data, int8_t my_id);
		
		void pack_resync(BitStream & data);
		void unpack_resync(BitStream & _data);
		
		
		// editor related
		void convert_gameplay_objects_into_map_format();
		void convert_map_objects_into_gameplay_format();
		
		void resize_map_in_all_directions(int up, int down, int left, int right);
		void move_all_objects(int tiles_h, int tiles_v);
		void remove_out_of_bounds_objects();
		
};
#endif
