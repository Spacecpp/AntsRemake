#include "GameMap.h"
#include "globals.h"
#include "lists.h"

#if defined(MAP_LOAD_DEBUG)
#define DEBUG_PRINTF printf
#else
#define DEBUG_PRINTF(format, args...) ((void)0)
#endif

bool Tile::has_landmine()
{
    return (secondary >= SECONDARY_TILE::LANDMINE_0 && secondary < SECONDARY_TILE::LANDMINE_0 + MAX_PLAYERS);
}


bool Tile::has_landmine(uint8_t ownerIndex)
{
    return (secondary == (SECONDARY_TILE::LANDMINE_0 + ownerIndex) );
}


int Tile::get_move_cost()
{
	return (type == TILE::MUD ? 2 : 1);
}



GameMap::GameMap(): loaded(false), wasModified(false), width(0), height(0), gameTime(0), eggs(-1), defaultPowerup(0)
{
	renderCache.create_sprite_cache();
}


GameMap::~GameMap()
{
	renderCache.destroy();
}


void GameMap::reset()
{
	
	loaded = false;
	wasModified = false;
	
	gameTime = 0;
	
	description.clear();
	
	eggs = -1;
	defaultPowerup = 0;
	
	name.clear();
	fullFilePath.clear();
	
	reset_spawners();
	
	reset_tiles();
	
	renderCache.clear_cache();
	
}


void GameMap::clear_cache()
{
	renderCache.clear_cache();
}


void GameMap::change_size(int w, int h)
{
	
	if (w < 1 || h < 1) return;
	
	//Tile base_tile();
	
	if (h != height) 
	{
		
		terrain.resize(h);
		height = h;
		
	}
	
	//if (w != width) // sempre vai dar este resize
	{
		
		for (int y = 0; y < height; y++) 
			terrain[y].resize(w, Tile() );
		
		width = w;
		
	}
	
}



void GameMap::resize_in_all_directions(int up, int down, int left, int right)
{
	
	// comeca adicionando / removendo na esquerda e topo, depois direita e baixo usando a funcao mais simples
	
	// se for 0 todo o calculo é desnecessario, se a soma nao for > 0 entao vai sumir com o mapa ou ate travar
	if (left != 0 && width + left > 0)
	{
	
		width += left;
	
		const bool remove = left < 0;
		
		Tile base_tile;
	
		for (size_t y = 0; y < terrain.size(); y++) 
		{
			
			for (int x = 0; x < abs(left); x++) 
			{
				
				if (remove)
					terrain[y].erase( terrain[y].begin() );
					
				else
					terrain[y].insert( terrain[y].begin(), base_tile); 
				
			}
		
		}
	
	}
	
	
	
	if (up != 0 && height + up > 0)
	{
		
		height += up;
	
		const bool remove = up < 0;
		
		Tile base_tile;
		
		for (int y = 0; y < abs(up); y++) 
		{
			
			if (remove)
				terrain.erase( terrain.begin() );
				
			else
			{
				terrain.insert( terrain.begin(), std::vector<Tile>() ); 
				terrain[0].resize(width, base_tile);
			}
			
		}		
	
	}	
	
	change_size(width + right, height + down);
	
	
	// ajusta posicao de objetos
	if (up != 0 || left != 0)
    {
		
		for (AntHillSpawn & current : antHillSpawn) 
        {
            if (up != 0)
                current.y += up;
                
            if (left != 0)
                current.x += left;
        }
        
        for (AntSpawn & current : antSpawn) 
        {
            if (up != 0)
                current.y += up;
                
            if (left != 0)
                current.x += left;
        }
        
        for (FoodSpawn & current : foodSpawn) 
        {
            if (up != 0)
                current.y += up;
                
            if (left != 0)
                current.x += left;
        }
        
        for (ItemSpawn & current : itemSpawn) 
        {
            if (up != 0)
                current.y += up;
                
            if (left != 0)
                current.x += left;
        }
        
        for (PowerupGeneratorSpawn & current : powerupGeneratorSpawn) 
        {
            if (up != 0)
                current.y += up;
                
            if (left != 0)
                current.x += left;
        }
        
    }
	
}



void GameMap::reset_tiles()
{
	for (int y = 0; y < height; y++) 
	{
        for (int x = 0; x < width; x++) 
		{
            terrain[y][x].reset();
        }
    }
}


void GameMap::reset_pathmap()
{
	
	for (int y = 0; y < height; y++) 
	{
        for (int x = 0; x < width; x++) 
		{
            terrain[y][x].path = false;
        }
    }
	
}


void GameMap::reset_spawners()
{
	
	antHillSpawn.clear();
	antSpawn.clear();
	foodSpawn.clear();
	itemSpawn.clear();
	obstacleSpawn.clear();
	powerupGeneratorSpawn.clear();
	
}


void GameMap::set_fire(int x, int y, int8_t value)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		
		terrain[y][x].value = value;
		terrain[y][x].secondary = value > 0 ? SECONDARY_TILE::FIRE : SECONDARY_TILE::NONE;
		//terrain[y][x].fire = value;
	}
	
}



void GameMap::set_mud_bridge(int x, int y, int8_t value)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		
		terrain[y][x].value = value;
		terrain[y][x].secondary = value > 0 ? SECONDARY_TILE::MUD_BRIDGE : SECONDARY_TILE::NONE;
	}
	
}



void GameMap::deploy_land_mine(int x, int y, uint8_t ownerIndex)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		
		terrain[y][x].value = 1;
		terrain[y][x].secondary = SECONDARY_TILE::LANDMINE_0 + ownerIndex;
	}
	
}



void GameMap::remove_land_mine(int x, int y)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		
		terrain[y][x].value = 0;
		terrain[y][x].secondary = SECONDARY_TILE::NONE;
	}
	
}



void GameMap::set_secondary_object(int x, int y, int8_t id)
{
    if (!coordinates_out_of_bounds(x, y) )
		terrain[y][x].secondary = id;
}



void GameMap::remove_all_secondary_objects()
{
    for (int y = 0; y < height; y++) 
	{
        for (int x = 0; x < width; x++) 
		{
            terrain[y][x].secondary = SECONDARY_TILE::NONE;
        }
    }
}



bool GameMap::set_path(int x, int y, bool value)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		terrain[y][x].path = value;
		return true;
	}
	return false;
	
}



int8_t GameMap::get_fire(int x, int y)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		
		if (terrain[y][x].secondary == SECONDARY_TILE::FIRE)
			return terrain[y][x].value;
		//return terrain[y][x].fire;
	}
	return 0;
	
}



int8_t GameMap::get_landmine(int x, int y)
{
	
	if (!coordinates_out_of_bounds(x, y) )
	{
		
		if (terrain[y][x].secondary == SECONDARY_TILE::LANDMINE_0)
			return terrain[y][x].value;
		
	}
	return 0;
	
}



void GameMap::limit_size(int limit)
{
	
	const int new_width = width > limit ? limit : width;
	const int new_height = height > limit ? limit : height;
	
	if (new_width > limit || new_height > limit)
		change_size(new_width, new_height);
	
}


bool GameMap::coordinates_out_of_bounds(int x, int y)
{
	return (x < 0 || y < 0 || x >= width || y >= height);
}


bool GameMap::world_coordinates_out_of_bounds(float x, float y)
{
	return (x < 0.0f || y < 0.0f || x >= width * TILE_SIZE || y >= height * TILE_SIZE);
}



int8_t GameMap::get_tile_type(int x, int y)
{
	
	if (!coordinates_out_of_bounds(x, y) )
		return terrain[y][x].type;
	return 0;
}



Tile * GameMap::get_tile(int x, int y)
{
	
	if (!coordinates_out_of_bounds(x, y) )
		return &terrain[y][x];
	return NULL;
	
}


bool GameMap::tile_is_open_for_fire_or_bomb(int x, int y)
{
	int8_t type = get_tile_type(x, y);
	return (type == TILE::DIRT || type == TILE::SAND || type == TILE::ROCKY_GROUND);
}


bool GameMap::tile_is_solid(int x, int y)
{
	if (!coordinates_out_of_bounds(x, y) )
		return terrain[y][x].solid;
	return false;
}



void GameMap::reset_solid_marks()
{
	
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
			terrain[y][x].solid = false;
	}
	
}


void GameMap::mark_solid_tile(int x, int y)
{
	if (!coordinates_out_of_bounds(x, y) )
		terrain[y][x].solid = true;
}


void GameMap::mark_solid_tiles(int left, int top, int right, int bottom)
{
	
	// safety 
	if (  left < 0) left = 0;   if (  left >= width ) left = width - 1;
	if (   top < 0) top = 0;    if (   top >= height) top = height - 1;
	if ( right < 0) right = 0;  if ( right >= width ) right = width - 1;
	if (bottom < 0) bottom = 0; if (bottom >= height) bottom = height - 1;

	for (int y = top; y <= bottom; y++) 
	{
		for (int x = left; x <= right; x++) 
			terrain[y][x].solid = true;
	}
	
}



bool GameMap::tile_occupied_by_object(int x, int y)
{
	
	for (const AntHillSpawn & current : antHillSpawn) 
	{
		if (current.x == x && current.y == y)
			return true;
	}
	
	for (const AntSpawn & current : antSpawn) 
	{
		if (current.x == x && current.y == y)
			return true;
	}
	
	for (const FoodSpawn & current : foodSpawn) 
	{
		if (current.x == x && current.y == y)
			return true;
	}
	
	for (const ItemSpawn & current : itemSpawn) 
	{
		if (current.x == x && current.y == y)
			return true;
	}
	
	return false;
	
}


/*
bool GameMap::remove_object_at_tile(int x, int y)
{
	
	size_t i = 0;
	for (const IntVector3 & current : antHillSpawn) 
	{
		
		if (current.x == x && current.y == y)
		{
			antHillSpawn.erase( antHillSpawn.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const IntVector3 & current : antSpawn) 
	{
		
		if (current.x == x && current.y == y)
		{
			antSpawn.erase( antSpawn.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const IntVector3 & current : foodSpawn) 
	{
		
		if (current.x == x && current.y == y)
		{
			foodSpawn.erase( foodSpawn.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const IntVector3 & current : itemSpawn) 
	{
		
		if (current.x == x && current.y == y)
		{
			itemSpawn.erase( itemSpawn.begin() + i);
			return true;
		}
		i++;
		
	}
	
	i = 0;
	for (const IntVector3 & current : obstacleSpawn) 
	{
		
		if (current.x == x && current.y == y)
		{
			obstacleSpawn.erase( obstacleSpawn.begin() + i);
			return true;
		}
		i++;
		
	}
	
	return false;
	
}*/


void GameMap::draw()
{
	
	if (renderCache.is_empty() )
	{
		
		const float src_w = 32.0f;
		const float src_h = 32.0f;
		const float dst_w = TILE_SIZE;
		const float dst_h = TILE_SIZE;
		
		for (int y = 0; y < height; y++) 
		{
			for (int x = 0; x < width; x++) 
			{
				
				Tile & currentTile = terrain[y][x];
				const int8_t type = currentTile.type;
				
				const float draw_x = x * (float)TILE_SIZE;
				const float draw_y = y * (float)TILE_SIZE;
				
				if (type != TILE::WATER) // agua eh desenhado de outra forma
				{
					
					Bitmap * bitmap = NULL;
					
					if (type == TILE::DIRT)
						bitmap = &dirtTexture;
						
					else if (type == TILE::SAND)
						bitmap = &sandTexture;
					
					else if (type == TILE::ROCKY_GROUND)
						bitmap = &rockyGroundTexture;
					
					else if (type == TILE::MUD)
						bitmap = &mudTexture;
						
					else if (type == TILE::_VOID_)
						bitmap = &voidTileTexture;
					
					if (bitmap)
						renderCache.draw_sprite_region(*bitmap, 0.0f, 0.0f, src_w, src_h, draw_x, draw_y, dst_w, dst_h);
					
				}
				
				
				if (currentTile.sandVariant != 0)
				{
					
					const float src_x = (float)currentTile.sandVariant * 32.0f;
					Bitmap & sprite = sandTransitionTileSprite;
					
					renderCache.draw_sprite_region(sprite, src_x, 0.0f, src_w, src_h, draw_x, draw_y, dst_w, dst_h);
					
				}
				
				if (currentTile.rockyVariant != 0)
				{
					
					const float src_x = (float)currentTile.rockyVariant * 32.0f;
					Bitmap & sprite = rockyTransitionTileSprite;
					
					renderCache.draw_sprite_region(sprite, src_x, 0.0f, src_w, src_h, draw_x, draw_y, dst_w, dst_h);
					
				}
				
				if (currentTile.mudVariant != 0)
				{
					
					const float src_x = (float)currentTile.mudVariant * 32.0f;
					Bitmap & sprite = type == TILE::WATER ? mudToWaterTransitionTileSprite : mudTransitionTileSprite;
					
					renderCache.draw_sprite_region(sprite, src_x, 0.0f, src_w, src_h, draw_x, draw_y, dst_w, dst_h);
					
				}
				
				
			}
		}
	
	}
	
	renderCache.draw();
	
}



void GameMap::calculate_tile_variants(int left, int top, int right, int bottom)
{
	
	if (left < 0) left = 0;
	if (top < 0) top = 0;
	if (right > width - 1) right = width - 1;
	if (bottom > height - 1) bottom = height - 1;
	
	const bool dontLinkAtBorders = false;
	
	for (int y = top; y <= bottom; y++)  
	{

		for (int x = left; x <= right; x++)    
		{
			
			Tile & currentTile = terrain[y][x];
			const char id = currentTile.type;
			
			currentTile.mudVariant = 0;
			currentTile.sandVariant = 0;
			currentTile.rockyVariant = 0;
			
			if (id != TILE::MUD)
			{
				
				//char variant = 0, variant2 = 0, variant3 = 0;
				char tile[4];
				
				// checa os bounds, se cair fora pegar igual, exceto se for dontLinkAtBorders
				if (y - 1 >= 0    ) tile[0] = terrain[y-1][x].type; else tile[0] = dontLinkAtBorders ? TILE::_VOID_ : id;
				if (x + 1 < width ) tile[1] = terrain[y][x+1].type; else tile[1] = dontLinkAtBorders ? TILE::_VOID_ : id;
				if (y + 1 < height) tile[2] = terrain[y+1][x].type; else tile[2] = dontLinkAtBorders ? TILE::_VOID_ : id;
				if (x - 1 >= 0    ) tile[3] = terrain[y][x-1].type; else tile[3] = dontLinkAtBorders ? TILE::_VOID_ : id;
				
				if (id == TILE::WATER)
				{
					
					if (tile[0] != id) currentTile.mudVariant += 1;
					if (tile[1] != id) currentTile.mudVariant += 2;
					if (tile[2] != id) currentTile.mudVariant += 4;
					if (tile[3] != id) currentTile.mudVariant += 8;
					
				}
				
				else
				{
					
					if (tile[0] == TILE::MUD) currentTile.mudVariant += 1;
					if (tile[1] == TILE::MUD) currentTile.mudVariant += 2;
					if (tile[2] == TILE::MUD) currentTile.mudVariant += 4;
					if (tile[3] == TILE::MUD) currentTile.mudVariant += 8;
					
					if (tile[0] == TILE::SAND && id < tile[0]) currentTile.sandVariant += 1;
					if (tile[1] == TILE::SAND && id < tile[1]) currentTile.sandVariant += 2;
					if (tile[2] == TILE::SAND && id < tile[2]) currentTile.sandVariant += 4;
					if (tile[3] == TILE::SAND && id < tile[3]) currentTile.sandVariant += 8;
					
					if (tile[0] == TILE::ROCKY_GROUND && id != tile[0]) currentTile.rockyVariant += 1;
					if (tile[1] == TILE::ROCKY_GROUND && id != tile[1]) currentTile.rockyVariant += 2;
					if (tile[2] == TILE::ROCKY_GROUND && id != tile[2]) currentTile.rockyVariant += 4;
					if (tile[3] == TILE::ROCKY_GROUND && id != tile[3]) currentTile.rockyVariant += 8;
					
				}
				
			}
			
		}
		
	}
	
}


void GameMap::calculate_tile_variants()
{
	calculate_tile_variants(0, 0, width - 1, height - 1);	
}



void GameMap::remove_out_of_bounds_objects()
{
	
	for (std::vector<AntHillSpawn>::iterator it = antHillSpawn.begin(); it != antHillSpawn.end(); )
	{
		if (coordinates_out_of_bounds(it->x, it->y) )
			it = antHillSpawn.erase(it);
		else
			++it;
	}
	
	for (std::vector<AntSpawn>::iterator it = antSpawn.begin(); it != antSpawn.end(); )
	{
		if (coordinates_out_of_bounds(it->x, it->y) )
			it = antSpawn.erase(it);
		else
			++it;
	}
	
	for (std::vector<FoodSpawn>::iterator it = foodSpawn.begin(); it != foodSpawn.end(); )
	{
		if (coordinates_out_of_bounds(it->x, it->y) )
			it = foodSpawn.erase(it);
		else
			++it;
	}
	
	for (std::vector<ItemSpawn>::iterator it = itemSpawn.begin(); it != itemSpawn.end(); )
	{
		if (coordinates_out_of_bounds(it->x, it->y) )
			it = itemSpawn.erase(it);
		else
			++it;
	}
	
	for (std::vector<ObstacleSpawn>::iterator it = obstacleSpawn.begin(); it != obstacleSpawn.end(); )
	{
		if (coordinates_out_of_bounds(it->x, it->y) )
			it = obstacleSpawn.erase(it);
		else
			++it;
	}
	
	for (std::vector<PowerupGeneratorSpawn>::iterator it = powerupGeneratorSpawn.begin(); it != powerupGeneratorSpawn.end(); )
	{
		if (coordinates_out_of_bounds(it->x, it->y) )
			it = powerupGeneratorSpawn.erase(it);
		else
			++it;
	}
	
}



uint64_t GameMap::get_checksum()
{
	
	uint64_t sum = 0;
	
	sum += gameTime;
	sum += width; 
	sum += height;
	
	int8_t prev = 0;
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{
			
			int8_t current = terrain[y][x].type;
			sum += current + 1;
			
			if (prev != current)
			{
				prev = current;
				sum += x + y;
			}
		}
	}
	
	for (AntHillSpawn & current : antHillSpawn)
		sum += (current.x + current.y + current.z + current.w);
	
	for (AntSpawn & current : antSpawn)
		sum += (current.x + current.y + current.z);
		
	for (FoodSpawn & current : foodSpawn)
		sum += (current.x + current.y + current.z);
	
	for (ItemSpawn & current : itemSpawn)
		sum += (current.x + current.y + current.z);
	
	for (ObstacleSpawn & current : obstacleSpawn)
		sum += (current.x + current.y + current.z);
	
	for (PowerupGeneratorSpawn & current : powerupGeneratorSpawn)
		sum += (current.x + current.y + current.z + current.w);
	
	return sum;
	
}


String GameMap::get_name()
{
	
	if (!name.empty() )
		return name;
	
	if (!fileName.empty() )
		return fileName;
	
	return "?";
	
}


// save load
bool GameMap::save(const char * filename)
{
	
	
	FileHandler file;
	if (file.create(filename, true) )
	{
	    
	    String header = MAP_FILE_HEADER;
	    file.write_Data(header.data(), header.size() );
	    
	    file.write_variable(MAP_FILE_VERSION);
	    
	    // ---
	    file.write_variable(name);
	    file.write_variable(gameTime);
	    file.write_variable(eggs);
	    file.write_variable(defaultPowerup);
		file.write_variable(description);
		
		// terreno
		file.write_variable(width);
		file.write_variable(height);
		
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				file.write_variable(terrain[y][x].type);
			}
		}
		
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				file.write_variable(terrain[y][x].secondary);
			}
		}
		
		//objetos
		file.write_variable( (uint32_t)antHillSpawn.size() );
		for (const AntHillSpawn & current : antHillSpawn) 
		{
			
			file.write_variable(current.x);
			file.write_variable(current.y);
			file.write_variable(current.z);
			file.write_variable(current.w);
			
		}
		
		file.write_variable( (uint32_t)antSpawn.size() );
		for (const AntSpawn & current : antSpawn) 
		{
			
			file.write_variable(current.x);
			file.write_variable(current.y);
			file.write_variable(current.z);
			
		}
		
		file.write_variable( (uint32_t)foodSpawn.size() );
		for (const FoodSpawn & current : foodSpawn) 
		{
			
			file.write_variable(current.x);
			file.write_variable(current.y);
			file.write_variable(current.z);
			
		}
		
		file.write_variable( (uint32_t)itemSpawn.size() );
		for (const ItemSpawn & current : itemSpawn) 
		{
			
			file.write_variable(current.x);
			file.write_variable(current.y);
			file.write_variable(current.z);
			
		}
		
		file.write_variable( (uint32_t)obstacleSpawn.size() );
		for (const ObstacleSpawn & current : obstacleSpawn) 
		{
			
			file.write_variable(current.x);
			file.write_variable(current.y);
			file.write_variable(current.z);
			
		}
		
		file.write_variable( (uint32_t)powerupGeneratorSpawn.size() );
		for (const PowerupGeneratorSpawn & current : powerupGeneratorSpawn) 
		{
			
			file.write_variable(current.x);
			file.write_variable(current.y);
			file.write_variable(current.z);
			file.write_variable(current.w);
			
		}
		
		// finaliza
		file.close();
		
		return true;
		
	}
	
	return false;

}



bool GameMap::load(const char * filename)
{
	
	reset();
	
	FileHandler file;
	if (file.open(filename, true) )
	{
		
		DEBUG_PRINTF("\nMap load began\n");
		
		char header[32];
		memset(header, 0, 32);
		size_t bytes_to_read = strlen(MAP_FILE_HEADER);
		bool header_read = file.read_Data(header, bytes_to_read);
		if (header_read)
		{
			DEBUG_PRINTF("header: %s\n", header);
		}
		
		int version = 0;
		file.read_variable(version);
		DEBUG_PRINTF("version: %d\n", version);
		
		// ---
		file.read_variable(name);
	    file.read_variable(gameTime);
	    file.read_variable(eggs);
	    file.read_variable(defaultPowerup);
	    file.read_variable(description);
	    
	    DEBUG_PRINTF("gameTime: %d, eggs: %d, defaultPowerup: %d, description: %s\n", gameTime, (int)eggs, (int)defaultPowerup, description.c_str() );
	    
		// terreno
		int w = 0, h = 0;
		file.read_variable(w);
		file.read_variable(h);
		DEBUG_PRINTF("width: %d, height %d\n", w, h);
		change_size(w, h);
		
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				file.read_variable(terrain[y][x].type);
				//DEBUG_PRINTF("%d ", terrain[y][x].type);
			}
		}
		
		if (version >= 2)
		{
			for (int y = 0; y < h; y++)
			{
				for (int x = 0; x < w; x++)
				{
					file.read_variable(terrain[y][x].secondary);
					//DEBUG_PRINTF("%d ", terrain[y][x].type);
				}
			}
		}
		
		//DEBUG_PRINTF("\n");
		
		// objetos
		// anthill
		uint32_t count = 0;
		file.read_variable(count);
		DEBUG_PRINTF("\nanthill count: %u\n", (unsigned int)count);
		for (size_t i = 0; i < count; i++)
		{
			
			IntVector4 vec;
			file.read_variable(vec.x);
			file.read_variable(vec.y);
			file.read_variable(vec.z);
			file.read_variable(vec.w);
			//DEBUG_PRINTF("x: %d, y: %d, z: %d\n", vec.x, vec.y, vec.z);
			antHillSpawn.push_back(vec);
			
		}
		
		// ant
		count = 0;
		file.read_variable(count);
		DEBUG_PRINTF("\nant count: %u\n", (unsigned int)count);
		for (size_t i = 0; i < count; i++)
		{
			
			IntVector3 vec;
			file.read_variable(vec.x);
			file.read_variable(vec.y);
			file.read_variable(vec.z);
			DEBUG_PRINTF("x: %d, y: %d, z: %d\n", vec.x, vec.y, vec.z);
			antSpawn.push_back(vec);
			
		}
		
		// food
		count = 0;
		file.read_variable(count);
		DEBUG_PRINTF("\nfood count: %u\n", (unsigned int)count);
		for (size_t i = 0; i < count; i++)
		{
			
			IntVector3 vec;
			file.read_variable(vec.x);
			file.read_variable(vec.y);
			file.read_variable(vec.z);
			DEBUG_PRINTF("x: %d, y: %d, z: %d\n", vec.x, vec.y, vec.z);
			foodSpawn.push_back(vec);
			
		}
		
		// powerup
		count = 0;
		file.read_variable(count);
		DEBUG_PRINTF("\npowerup count: %u\n", (unsigned int)count);
		for (size_t i = 0; i < count; i++)
		{
			
			IntVector3 vec;
			file.read_variable(vec.x);
			file.read_variable(vec.y);
			file.read_variable(vec.z);
			DEBUG_PRINTF("x: %d, y: %d, z: %d\n", vec.x, vec.y, vec.z);
			
			// possivel correcao
			if (vec.z < 1)
				vec.z = 1;
			
			itemSpawn.push_back(vec);
			
		}
		
		// obstacle
		count = 0;
		file.read_variable(count);
		DEBUG_PRINTF("\nobstacle count: %u\n", (unsigned int)count);
		for (size_t i = 0; i < count; i++)
		{
			
			IntVector3 vec;
			file.read_variable(vec.x);
			file.read_variable(vec.y);
			file.read_variable(vec.z);
			DEBUG_PRINTF("x: %d, y: %d, z: %d\n", vec.x, vec.y, vec.z);
			obstacleSpawn.push_back(vec);
			
		}
		
		// powerup generator
		count = 0;
		file.read_variable(count);
		DEBUG_PRINTF("\npowerup generator count: %u\n", (unsigned int)count);
		for (size_t i = 0; i < count; i++)
		{
			
			IntVector4 vec;
			file.read_variable(vec.x);
			file.read_variable(vec.y);
			file.read_variable(vec.z);
			file.read_variable(vec.w);
			//DEBUG_PRINTF("x: %d, y: %d, z: %d, w: %d\n", vec.x, vec.y, vec.z, vec.w);
			powerupGeneratorSpawn.push_back(vec);
			
		}
		
		
		fullFilePath = filename;
		this->fileName = get_file_basename(fullFilePath);
		//name = get_file_basename(fullFilePath);
		calculate_tile_variants();
		remove_out_of_bounds_objects();
		loaded = true;
		wasModified = false;
		
		// finaliza
		file.close();
		DEBUG_PRINTF("load end\n\n");
		
		return true;
		
	}
	
	return false;

}


