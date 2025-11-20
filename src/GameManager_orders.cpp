#include "GameManager.h"

void GameManager::update_order_queue()
{
    
    // processa as ordens que estao na fila
    if (!queuedOrder.empty() )
    {
        
        //for (GameOrder & currentOrder : queuedOrder)
        for (std::list<GameOrder>::iterator it = queuedOrder.begin(); it != queuedOrder.end(); )
        {
            
            GameOrder & currentOrder = *it;
            
            // esta no frame certo? executa e tira da lista
            if (currentOrder.targetGameFrame == gameFrame)
            {
                execute_order(currentOrder);
                it = queuedOrder.erase(it);
            }
            
            // atrasada, NAO deve acontecer!
            else if (currentOrder.targetGameFrame < gameFrame)
            {
                printf("Outdated order! %d, %d (%d frames)\n", (int)currentOrder.targetGameFrame, (int)gameFrame, (int)(gameFrame - currentOrder.targetGameFrame) );
                it = queuedOrder.erase(it);
            }
            
            else ++it;
            
        }
        
    }
    
}



void GameManager::execute_order(GameOrder & order)
{
	
	switch (order.id)
	{
		
		case ORDER_ID::ANT_MOVE:
		{
			
			std::vector<Ant*> targetAnts;
			get_Ants_by_ids(targetAnts, order.antUniqueID);
			
			order_ant_move(true, targetAnts, order.x, order.y, (bool)order.option);
			break;
			
		}
		
		case ORDER_ID::ATTACK:
		{
			
			if (Ant * defenderAnt = get_Ant(order.targetID) )
			{
				
				std::vector<Ant*> targetAnts;
				get_Ants_by_ids(targetAnts, order.antUniqueID);
			
				order_ant_attack(true, targetAnts, *defenderAnt);
			
			}
			else printf("Ant id %d not found\n", (int)order.targetID );
			
			break;
			
		}
		
		case ORDER_ID::USE_SPECIAL:
		{
			
			if (order.antUniqueID.size() == 1)
			{
				if (Ant * targetAnt = get_Ant(order.antUniqueID.front() ) )
				{
					
					std::vector<Ant*> targetAnts;
					get_Ants_by_ids(targetAnts, order.antUniqueID);
				
					order_ant_use_special_ability(true, *targetAnt, order.x, order.y, order.option);
				
				}
				else printf("Ant id %d not found\n", (int)order.antUniqueID.front() );
			}
			else printf("order.antUniqueID.size() == 1 FALSE\n");
			
			break;
			
		}
		
		case ORDER_ID::EGG_HATCH:
		{
			
			if (AntHill * current = get_AntHill(order.playerID) )
			{
			
				order_anthill_hatch_egg(true, *current);
				
			}
			else printf("Player id %d not found\n", (int)order.playerID);
			
			break;
			
		}
		
		default: printf("Unrecognized order id %d\n", (int)order.id);
		
	}
	
}



bool GameManager::order_ant_move(bool apply_now, std::vector<Ant*> & targetAnts, int x, int y, bool goingAfterFood)
{
	
	if (gameMap.coordinates_out_of_bounds(x, y) ) return false;
	
	int success = 0;
	
	for (Ant * c_ant_p : targetAnts) 
	{
		if (c_ant_p->active)
		{
			
			Ant & c_ant = *c_ant_p;
			
			if (tile_is_walkable(x, y, c_ant.get_movement_type() ) )
			{
				
				std::queue<IntVector2> temp_path;
				std::queue<IntVector2> & path = apply_now ? c_ant.path : temp_path;
				
				if (find_path(c_ant.get_tile_position(), IntVector2(x, y), path, c_ant.get_movement_type(), false, c_ant.get_owner_index() ) )
				{
					
					success++;
					
					if (apply_now)
					{
						
						c_ant.destination = IntVector2(x, y);
						c_ant.goingAfterFood = goingAfterFood;
						if (goingAfterFood)
                            c_ant.targetFoodPosition = c_ant.destination;
						c_ant.myAttackTarget = NULL;
						c_ant.myAttackTarget_id = 0;
						c_ant.goingToUseAbility = false;
						
					}
					
				}
			}	
		
		}
	}
	
	// manda pra fila
	if (success && !apply_now)
	{
		
		GameOrder order;
		order.init_ant_move(gameFrame + commandDelay * 2, targetAnts, x, y, goingAfterFood);
		
		std::list<GameOrder> & targetQueue = is_online() ? netQueuedOrder : queuedOrder;
		targetQueue.push_back(order);
		
	}
	
	return success > 0;
	
}



bool GameManager::order_ant_attack(bool apply_now, std::vector<Ant*> & targetAnts, Ant & defenderAnt)
{
	
	IntVector2 defenderTilePosition = defenderAnt.get_tile_position();
	
	if (gameMap.coordinates_out_of_bounds(defenderTilePosition.x, defenderTilePosition.y) ) return false;
	
	int success = 0;
	
	for (Ant * c_ant_p : targetAnts) 
	{
		if (c_ant_p->active)
		{
			
			Ant & c_ant = *c_ant_p;
			
			if (tile_is_walkable(defenderTilePosition.x, defenderTilePosition.y, c_ant.get_movement_type() ) )
			{
				
				std::queue<IntVector2> temp_path;
				std::queue<IntVector2> & path = apply_now ? c_ant.path : temp_path;
				
				if (find_path(c_ant.get_tile_position(), defenderTilePosition, path, c_ant.get_movement_type(), false, c_ant.get_owner_index() ) )
				{
					
					success++;
					
					if (apply_now)
					{
						
						c_ant.myAttackTarget = &defenderAnt;
						c_ant.myAttackTarget_id = defenderAnt.unique_id;
						c_ant.destination = defenderTilePosition;
						c_ant.goingAfterFood = false;
						c_ant.goingToUseAbility = false;
						
					}
					
				}
			}	
		
		}
	}
	
	// manda pra fila
	if (success && !apply_now)
	{
		
		GameOrder order;
		order.init_ant_attack(gameFrame + commandDelay * 2, targetAnts, defenderAnt);
		
		std::list<GameOrder> & targetQueue = is_online() ? netQueuedOrder : queuedOrder;
		targetQueue.push_back(order);
		
	}
	
	return success > 0;
	
}



bool GameManager::order_ant_use_special_ability(bool apply_now, Ant & targetAnt, int x, int y, int8_t option)
{
	
	if (gameMap.coordinates_out_of_bounds(x, y) ) return false;
	
	bool success = false;
	bool stop_one_before = true;
	
	// pra cad tipo de habilidade
	bool ok = false;
	if (targetAnt.currentPowerup == POWERUP::ROBBER)
    {
        
        printf("ant is thief\n");
        
        AntHill * targetAntHill = get_AntHill_at_tile(x, y);
        if (targetAntHill && targetAntHill->owner != targetAnt.owner)
        {
            
            ok = true;
            
            stop_one_before = false;
            
            IntVector2 tilePosition = targetAntHill->get_steal_hole_tile_position();
			x = tilePosition.x;
            y = tilePosition.y;
            
            printf("Thief order\n");
            
            /*
            // garante que o alvo eh o centro
            IntVector2 tilePosition = targetAntHill->get_tile_position();
            x = tilePosition.x;
            y = tilePosition.y;
            */
        }
        
    }
	
	else if (!tile_occupied_by_object(x, y) )
	{
		
		
		if (targetAnt.currentPowerup == POWERUP::FIREMAN)
			ok = gameMap.tile_is_open_for_fire_or_bomb(x, y);
		
		else if (targetAnt.currentPowerup == POWERUP::DIVER)
			ok = gameMap.get_tile_type(x, y) == TILE::WATER;
		
		else if (targetAnt.currentPowerup == POWERUP::BOMBER)
			ok = gameMap.tile_is_open_for_fire_or_bomb(x, y);
			
	}
	
	
	// calcula o pathfind
	if (ok)
    {
        
        std::queue<IntVector2> temp_path;
        std::queue<IntVector2> & path = apply_now ? targetAnt.path : temp_path;
        
        const int movement_type = targetAnt.currentPowerup == POWERUP::ROBBER ? MOVEMENT_TYPE::THIEF : targetAnt.get_movement_type();
        
        if (find_path(targetAnt.get_tile_position(), IntVector2(x, y), path, movement_type, stop_one_before, targetAnt.get_owner_index() ) )
        {
            
            success = true;
            
            if (apply_now)
            {
                
                targetAnt.destination = targetAnt.path.back(); //IntVector2(x, y);
                targetAnt.goingAfterFood = false;
                targetAnt.myAttackTarget = NULL;
                targetAnt.myAttackTarget_id = 0;
                targetAnt.abilityPosition = IntVector2(x, y);
                targetAnt.goingToUseAbility = option;
                
            }
            
        }
        else
		{
			if (targetAnt.currentPowerup == POWERUP::ROBBER)
				printf("thief pathfind fail\n");
		}
        
        
    }
	
	// manda pra fila
	if (success && !apply_now)
	{
		
		GameOrder order;
		order.init_ant_use_special_ability(gameFrame + commandDelay * 2, targetAnt, x, y, option);
		
		std::list<GameOrder> & targetQueue = is_online() ? netQueuedOrder : queuedOrder;
		targetQueue.push_back(order);
		
	}
	
	return success;
	
}



bool GameManager::order_anthill_hatch_egg(bool apply_now, AntHill & targetAntHill)
{
	
	bool success = false;
	
	if (targetAntHill.can_hatch_egg() )
	{
		
		success = true;
		
		if (apply_now)
			targetAntHill.hatch_egg();
		
	}
	
	// manda pra fila
	if (success && !apply_now)
	{
		
		GameOrder order;
		order.init_egg_hatch(gameFrame + commandDelay * 2, targetAntHill);
		
		std::list<GameOrder> & targetQueue = is_online() ? netQueuedOrder : queuedOrder;
		targetQueue.push_back(order);
		
	}
	
	return success;
	
}
