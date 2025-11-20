#ifndef GAMEMAP_H
#define GAMEMAP_H

#include "Alegrinho.h"
#include <vector>
#include <string>
#include <stdint.h>

#define MAP_FILE_VERSION 2
#define MAP_FILE_HEADER "ANTS_GAME"

class Tile 
{
	
	public:
		int8_t type;
		int8_t secondary;
		int8_t value;
		int8_t mudVariant;
		int8_t sandVariant;
		int8_t rockyVariant;
		bool path;
		bool solid;
		//int8_t fire;
		
		Tile(): type(0), secondary(0), value(0), mudVariant(0), sandVariant(0), rockyVariant(0), path(false), solid(false)
		{
		}
		
		void reset()
		{
			type = 0;
			secondary = 0;
			value = 0;
			mudVariant = 0;
			sandVariant = 0;
			rockyVariant = 0;
			path = false;
			solid = false;
		}
		
		bool has_landmine();
		bool has_landmine(uint8_t ownerIndex);
		
		int get_move_cost();
   
};


typedef IntVector4 AntHillSpawn;
typedef IntVector3 AntSpawn;
typedef IntVector3 FoodSpawn;
typedef IntVector3 ItemSpawn;
typedef IntVector3 ObstacleSpawn;
typedef IntVector4 PowerupGeneratorSpawn;


class GameMap
{
	
	private:
	
	public:
		bool loaded;
		bool wasModified;
		int width, height;
		std::string name;
		std::string fileName;
		std::string fullFilePath;
		String description;
		int gameTime;
		int16_t eggs;
		int8_t defaultPowerup;
		
		std::vector< std::vector<Tile> > terrain;
		
		std::vector<AntHillSpawn> antHillSpawn;
		std::vector<AntSpawn> antSpawn;
		std::vector<FoodSpawn> foodSpawn;
		std::vector<ItemSpawn> itemSpawn;
		std::vector<ObstacleSpawn> obstacleSpawn;
		std::vector<PowerupGeneratorSpawn> powerupGeneratorSpawn;
		
		RenderCache renderCache;
		
		
		GameMap();
		~GameMap();
		void reset();
		void clear_cache();
		void change_size(int w, int h);
		void resize_in_all_directions(int up, int down, int left, int right);
		
		void reset_tiles();
		void reset_pathmap();
		void reset_spawners();
		
		void set_fire(int x, int y, int8_t value);
		void set_mud_bridge(int x, int y, int8_t value);
		void deploy_land_mine(int x, int y, uint8_t ownerIndex);
		void remove_land_mine(int x, int y);
		
		void set_secondary_object(int x, int y, int8_t id);
        void remove_all_secondary_objects();
		
		bool set_path(int x, int y, bool value);
		
		int8_t get_fire(int x, int y);
		int8_t get_landmine(int x, int y);
		
		void limit_size(int limit);
		bool coordinates_out_of_bounds(int x, int y);
		bool world_coordinates_out_of_bounds(float x, float y);
		int8_t get_tile_type(int x, int y);
		Tile * get_tile(int x, int y);
		bool tile_is_open_for_fire_or_bomb(int x, int y);
		bool tile_is_solid(int x, int y);
		void reset_solid_marks();
		void mark_solid_tile(int x, int y);
		void mark_solid_tiles(int left, int top, int right, int bottom);
		
		bool tile_occupied_by_object(int x, int y);
		//bool remove_object_at_tile(int x, int y);
		
		void draw();
		
		void calculate_tile_variants(int left, int top, int right, int bottom);
		void calculate_tile_variants();
		
		void remove_out_of_bounds_objects();
		
		uint64_t get_checksum();
		
		String get_name();
		
		// save load
		bool save(const char * filename);
		bool load(const char * filename);

};

#endif
