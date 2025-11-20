#include "GameManager.h"

void GameManager::update_AI_players()
{
	
	const bool apply_orders_now = is_offline();
	
	for (Player & current_player : player)
	{
		
		if (current_player.active && current_player.controller == CONTROLLER::AI)
		{
			
			bool enraged = false;
			
			std::vector<Ant*> myAnts = get_player_ants(current_player);
			if (!myAnts.empty() )
			{
				
				AntHill * myAntHill = get_AntHill(current_player);
				IntVector2 myhillTilePosition = myAntHill ? myAntHill->get_hole_tile_position() : IntVector2(0, 0);
				
				// manda as formigas comecarem colhendo
				if (gameFrame == 0)
				{
					if (myAntHill)
					{
						
						if (Food * targetFood = get_closest_food(myhillTilePosition.x, myhillTilePosition.y) )
						{
							
							IntVector2 foodTilePosition = targetFood->get_tile_position();
							order_ant_move(apply_orders_now, myAnts, foodTilePosition.x, foodTilePosition.y, true);
							
							//printf("AI initial harvest\n");
							
						}
						else enraged = true;
					}
				}
				
				else // seguintes
				{
					
					for (Ant * currentAntP : myAnts )
					{
						
						Ant & currentAnt = *currentAntP;
						if (currentAnt.idle() )
						{
							
							if (currentAnt.carrying_food() )
							{
								
								// volta a ir pro formigueiro depositar
								std::vector<Ant*> ant_ { &currentAnt };
								order_ant_move(apply_orders_now, ant_, myhillTilePosition.x, myhillTilePosition.y, true);
								
								//printf("AI coming back deposit food\n");
								
							}
							
							else
							{
								
								// busca novas comidas
								IntVector2 myTilePosition = currentAnt.get_tile_position();
								if (Food * targetFood = get_closest_food(myTilePosition.x, myTilePosition.y) )
								{
									
									IntVector2 foodTilePosition = targetFood->get_tile_position();
									std::vector<Ant*> ant_ { &currentAnt };
									order_ant_move(apply_orders_now, ant_, foodTilePosition.x, foodTilePosition.y, true);
									
									//printf("AI new harvest\n");
									
								}
								else enraged = true;
							
							}
							
							if (enraged)
							{
								
								if (Ant * target = get_closest_enemy_ant(currentAnt) )
								{
									
									std::vector<Ant*> ant_ { &currentAnt };
									order_ant_attack(apply_orders_now, ant_, *target);
									
								}
								
							}
							
						}
						
					}
					
				}
			
			}
			
			
		}
		
	}

}
