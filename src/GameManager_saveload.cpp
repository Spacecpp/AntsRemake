#include "GameManager.h"
#include "FileParser.h"


bool GameManager::save_map(const char * filename)
{
	
	convert_gameplay_objects_into_map_format();
	
	bool result = gameMap.save(filename);
	
	if (result)
		printf("%s saved\n", filename);
	else
		printf("Error saving %s\n", filename);
	
	return result;

}


bool GameManager::load_map(const char * filename)
{
	
	bool result = gameMap.load(filename);
	
	if (result)
	{
		
		printf("%s loaded\n", filename);
		convert_map_objects_into_gameplay_format();
		calculate_solid_tiles(); // para mostrar o preview corretamente
		
	}
	else
		printf("Error loading %s\n", filename);
	
	return result;

}



bool GameManager::load_selected_map()
{
	
	std::string mappath = "Maps/" + selectedMapName + ".map";
	return load_map(mappath.c_str() );
	
}



bool GameManager::save_config()
{
	
	#ifdef PLATFORM_WEB
	const char * filename = "persistent/config.ini";
	#else
	const char * filename = "config.ini";
	#endif
	
	FileHandler file;
	if (file.create(filename, false) )
	{
		
		printf("Saving config...");
		
		file.write_variable(localPlayerName);
		file.write_endline();
		
		file.write_variable(joinIP);
		file.write_endline();
		
		file.write_variable(selectedMapName);
		file.write_endline();
		
		file.write_variable(commandDelay);
		
		file.write_variable(global_sound_volume);
		
		// fim
		file.close();
		printf(" done\n");
		
		#ifdef PLATFORM_WEB
		web_save_persistent_storage();
		#endif
		
		return true;
		
	}
	
	return false;
	
}



bool GameManager::load_config()
{
	
	#ifdef PLATFORM_WEB
	const char * filename = "persistent/config.ini";
	#else
	const char * filename = "config.ini";
	#endif
	
	FileHandler file;
	if (file.open(filename, false) )
	{
		
		printf("Loading config...");
		
		file.read_variable(localPlayerName);
		file.read_variable(joinIP);
		file.read_variable(selectedMapName);
		file.read_variable(commandDelay);
		file.read_variable(global_sound_volume);
		
		// possiveis correcoes
		global_sound_volume = glm::clamp(global_sound_volume, 0.0f, 1.0f);
		
		// fim
		file.close();
		printf(" done\n");
		
		return true;
		
	}
	
	return false;
	
}



bool GameManager::load_food_data()
{
	
	bool success = false;
	
	FILE * file = fopen("Data/food.txt", "r");
	if (file)
	{
		
		foodData.clear();
		
		FileParser parser(file);
		while (!parser.end_of_file() && !parser.error() ) 
		{
			
			FoodData fd;
			fd.load(parser);
			
			if (!fd.name.empty() )
				foodData.push_back(fd);
			
			//foodData.push_back();
			//foodData.back().load(parser);
			
		}
		
		for (FoodData & current : foodData)
			current.load_sprite();
		
		
		fclose(file);
		
		success = true;
		
		printf("Loaded %d food data\n", (int)foodData.size() );
		
	}
	else printf("Failed to load food data\n"); 
	
	return success;
	
}



bool GameManager::load_obstacle_data()
{
	
	bool success = false;
	
	FILE * file = fopen("Data/obstacles.txt", "r");
	if (file)
	{
		
		obstacleData.clear();
		
		FileParser parser(file);
		while (!parser.end_of_file() && !parser.error() ) 
		{
			
			ObstacleData od;
			od.load(parser);
			
			if (!od.name.empty() )
				obstacleData.push_back(od);
			
			//foodData.push_back();
			//foodData.back().load(parser);
			
		}
		
		for (ObstacleData & current : obstacleData)
			current.load_sprite();
		
		
		fclose(file);
		
		success = true;
		
		printf("Loaded %d obstacle data\n", (int)obstacleData.size() );
		
		// debug
		//for (ObstacleData & current : obstacleData)
		//	current.debug_printf();
		
		
	}
	else printf("Failed to load obstacle data\n"); 
	
	return success;
	
}



bool GameManager::load_animation_data()
{
	
	bool success = false;
	
	FILE * file = fopen("Data/animations.txt", "r");
	if (file)
	{
		
		FileParser parser(file);
		while (!parser.end_of_file() && !parser.error() ) 
		{
			
			parser.get_next_line();
			if (parser.key_and_value_ok() )
			{
				
				std::string key = string_to_lowercase(parser.key);
				std::string value = string_to_lowercase(parser.value);
				//bool bad_value = false;
				
				if (key == "explosionanimationframes")
					global_explosionAnimationFrames = string_to_int(value);
				
				else if (key == "explosionanimationdelay")
					global_explosionAnimationDelay = string_to_int(value);
				
				
				if (key == "fireanimationframes")
					global_fireAnimationFrames = string_to_int(value);
				
				else if (key == "fireanimationdelay")
					global_fireAnimationDelay = string_to_int(value);
			}
			
		}
		
		if (global_explosionAnimationFrames < 1)
			global_explosionAnimationFrames = 5;
		
		if (global_fireAnimationFrames < 1)
			global_fireAnimationFrames = 8;
		
		printf("explosionAnimationFrames: %d\n", global_explosionAnimationFrames);
		printf("explosionAnimationDelay: %d\n", global_explosionAnimationDelay);
		printf("fireAnimationFrames: %d\n", global_fireAnimationFrames);
		printf("fireAnimationDelay: %d\n", global_fireAnimationDelay);
		
		fclose(file);
		
		success = true;
		
	}
	
	return success;
	
}
