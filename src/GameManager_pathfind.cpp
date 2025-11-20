#include "GameManager.h"


class compareIntVector2
{
	
	public:
		bool operator() (const std::pair<float, IntVector2> & n1, const std::pair<float, IntVector2> & n2) const
		{
			return n1.first < n2.first;
		}
	
};


//-----------------

float GameManager::get_move_cost(const IntVector2 & a, const IntVector2 & b) 
{
    return std::sqrt( (float)(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2) ) );
}



bool GameManager::find_path( const IntVector2 & start, const IntVector2 & end_, std::queue<IntVector2> & path, int8_t movementType, bool oneBefore, uint8_t playerIndex) 
{
	
	if (gameMap.coordinates_out_of_bounds(start.x, start.y) ||
		gameMap.coordinates_out_of_bounds(end_.x, end_.y) )
		return false;
		
	bool success = false;
	
	//limpa fila antiga
	while (!path.empty() )
        path.pop();
    
	
    // reset
	gameMap.reset_pathmap();
    const int map_width = gameMap.width;
	const int map_height = gameMap.height;
	
    // A*
    std::priority_queue< std::pair<float, IntVector2>, std::vector< std::pair<float, IntVector2> >, compareIntVector2 > pq;
    std::vector< std::vector<float> > cost(map_height, std::vector<float>(map_width, INFINITY) );
    std::vector< std::vector<IntVector2> > parent; 
	
	parent.resize(map_height);
	for (int i = 0; i < map_height; i++)
		parent[i].resize(map_width);
	
	
    pq.push( std::make_pair(0.0f, IntVector2(start.x, start.y) ) );
	
	if (!gameMap.coordinates_out_of_bounds(start.x, start.y) )
		cost[start.y][start.x] = 0.0f;
	
    while (!pq.empty() ) 
	{
		
        IntVector2 current_coordinates = pq.top().second;
        pq.pop();
		
		// chegou ao fim, construir o caminho
        if (current_coordinates == end_) 
		{
            
			// despreza o tile final
			if (oneBefore)
			{
				if (!gameMap.coordinates_out_of_bounds(current_coordinates.x, current_coordinates.y) )
					current_coordinates = parent[current_coordinates.y][current_coordinates.x];
			}
			
            while (current_coordinates != start) 
			{
				
                path.push(current_coordinates);
				gameMap.set_path(current_coordinates.x, current_coordinates.y, true);
				
				if (!gameMap.coordinates_out_of_bounds(current_coordinates.x, current_coordinates.y) )
					current_coordinates = parent[current_coordinates.y][current_coordinates.x];
				
            }
			
            path.push(start);
			reverse_queue(path);
			success = true;
            break;
			
        }
		
        for (int dx = -1; dx <= 1; dx++) 
		{
            for (int dy = -1; dy <= 1; dy++) 
			{
				
				int nx = current_coordinates.x + dx;
				int ny = current_coordinates.y + dy;
				
				// bound check
				if (nx >= 0 && nx < map_width && ny >= 0 && ny < map_height) 
				{
					
					if (Tile * currentTile = gameMap.get_tile(nx, ny) )
                    {
                        
                        if (tile_is_walkable(*currentTile, movementType) || (movementType == MOVEMENT_TYPE::THIEF && (end_.x == nx && end_.y == ny) ) )
                        {
                            
                            // impede de transitar se for mina do mesmo jogador, exceto se tiver mandado ir pra la
                            if ( !currentTile->has_landmine(playerIndex) || (end_.x == nx && end_.y == ny) )
                            {
                            
                                // calcula o custo atual e o custo estimado do próximo tile
                                //float currentCost = cost[current_coordinates.y][current_coordinates.x] + 1.0f;
                                
                                float currentCost = cost[current_coordinates.y][current_coordinates.x] + currentTile->get_move_cost();
                                float moveCost = get_move_cost( IntVector2(nx, ny), current_coordinates );
                                
                                float totalCost = currentCost + moveCost;
                                
                                // verifica se encontrou um caminho mais curto para o próximo tile
                                if (totalCost < cost[ny][nx]) 
                                {
                                    // atualiza os valores do custo e do pai do próximo tile
                                    cost[ny][nx] = totalCost;
                                    parent[ny][nx] = current_coordinates;
                                    
                                    // adiciona o próximo tile a fila de prioridade com a prioridade sendo a soma dos custos
                                    pq.push(std::make_pair( -totalCost, IntVector2 { nx, ny } ) );
                                }
                            
                            }
                            
                        }
                        
                    }
                    
				}
				
			}
        }
    }
	
	return success;
	
}
