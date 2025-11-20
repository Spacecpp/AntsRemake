#include "Ant.h"
#include "Food.h"
#include "Powerup.h"
#include "AntHill.h"
#include "Player.h"
#include "globals.h"
#include "lists.h"
#include "GameManager.h"
#include "General_func.h"
#include "Alegrinho.h"


Ant::Ant(): 
	GameObject(), position_z(0.0f), oldPosition_z(0.0f), oldPosition(position), velocity{ 0.0f, 0.0f, 0.0f}, pushBlocked(false), IHaveTheAnthillLock(false),
	angle(0.0f), visualAngle(0.0f), /*haveFood(false)*/ carriedFood(0), carriedFoodColor { 0, 0, 0, 0}, goingAfterFood(false), goingToUseAbility(false), enteringStealHole(false), HP(0), currentPowerup(0), animationFrame(0), animationTime(0), animationType(0), actionDelay(0), unique_id(0), owner_id(0), myAttackTarget_id(0), groupID(0), owner(NULL), myAttackTarget(NULL)
{
}

Ant::Ant(int x, int y, Player * player, int16_t unique_id, int8_t powerup): 
	GameObject(x, y), position_z(0.0f), oldPosition_z(0.0f), oldPosition(position), velocity{ 0.0f, 0.0f, 0.0f}, pushBlocked(false), IHaveTheAnthillLock(false),
	angle(0.0f), visualAngle(0.0f), /*haveFood(false)*/ carriedFood(0), carriedFoodColor { 0, 0, 0, 0}, goingAfterFood(false), goingToUseAbility(false), enteringStealHole(false), HP(10), currentPowerup(powerup), animationFrame(0), animationTime(0), animationType(0), actionDelay(0), unique_id(unique_id), owner_id(player ? player->id : 0), myAttackTarget_id(0), groupID(0), owner(player), myAttackTarget(NULL)
{
	
	/*
	if (owner)
		printf("Ant owner id: %d\n", owner->id);
	else
		printf("Owner is NULL\n");
	*/
}



void Ant::update()
{
	
	oldPosition = position;
	oldPosition_z = position_z;
	
	bool moved = false;
	
	AntHill * myHill = get_my_AntHill();
	
	bool grounded = position_z <= 0.0f;
	const bool was_grounded_previous_frame = grounded;
	
	// fisica primeiro
	if (velocity.x != 0.0f)
	{
		
		position.x += velocity.x;
		if (grounded)
		{
			velocity.x *= VELOCITY_DECAY_RATIO;
			if ( fabs(velocity.x) <= 0.2f)
				velocity.x = 0.0f;
		}
		
	}
	
	if (velocity.y != 0.0f)
	{
		
		position.y += velocity.y;
		if (grounded)
		{
			velocity.y *= VELOCITY_DECAY_RATIO;
			if ( fabs(velocity.y) <= 0.2f)
				velocity.y = 0.0f;
		}
		
	}
	
	if (velocity.z != 0.0f)
	{
		
		position_z += velocity.z;
		if (position_z <= 0.0f)
		{
			grounded = true;
			position_z = 0.0f;
			velocity.z = 0.0f;
		}
	}
	
	if (!grounded)
		velocity.z -= GRAVITY_FORCE;
	
	
	
	
	if (actionDelay > 0)
		actionDelay--;
	
	else
	{
		
		pushBlocked = false;
		
		// morte ocorre aqui
		if (HP <= 0)
		{
			die();
			return;
		}
		
		// checagem de ataque
		if (myAttackTarget)
		{
			
			bool abandon_target = false;
			if (myAttackTarget->active && myAttackTarget->HP > 0) // checa validade
			{
				
				// no alcance de ataque imediato
				if (in_attack_range(*myAttackTarget) )
                {
                    
                    if (myAttackTarget->can_be_hit() )
                    {
                        //hit(*myAttackTarget);
                        gameManager->fight.push_back( Fight(*this, *myAttackTarget) );
                    }
                    else abandon_target = true;
                    
                }
                
                // atualiza o path se necessario
                else
                {
                    
                    IntVector2 defenderTilePosition = myAttackTarget->get_tile_position();
                    if (this->destination != defenderTilePosition)
                    {
                        
                        if (gameManager->find_path(this->get_tile_position(), defenderTilePosition, this->path, this->get_movement_type(), false, this->get_owner_index() ) )
                        {
                            //printf("Updating path: %d, %d to %d, %d\n", destination.x, destination.y, defenderTilePosition.x, defenderTilePosition.y);
                            this->destination = defenderTilePosition;
                        }
                        else abandon_target = true;
                        
                    }
                    
                }
                
			}
			else abandon_target = true;
			
			if (abandon_target) // limpa
			{
				myAttackTarget = NULL;
				myAttackTarget_id = 0;
				clear_path();
			}
			
		}
		
		// Atualiza a posição se houver um caminho a seguir
		if (myHill && !this->path.empty() ) 
		{
			
			IntVector2 nextTile = this->path.front();
			
			// lixo
			/*
			if (this->x == nextTile.x && this->y == nextTile.x)
				this->path.pop();
			*/
			
			// condicao pra prosseguir
			//if (!gameManager->tile_occupied(nextTile.x, nextTile.y, this) )
				
			// se esta entrando no anthill
			const bool current_over_anthill = (myHill && myHill->ant_over_me(*this) );
			const bool next_over_anthill = (myHill && myHill->tile_over_me(nextTile.x, nextTile.y) );
			const bool entering_anthill = (next_over_anthill && !current_over_anthill);
		
			//const bool halt = (myHill && myHill->nearbyAnts > 0 && !myHill->ant_over_me(*this) && going_over_anthill);
			const bool halt = (entering_anthill && !IHaveTheAnthillLock && myHill && myHill->antLocked == true);
			
			
			if (halt)
			{
				pushBlocked = true;
				velocity = Vector3(0.0f, 0.0f, 0.0f);
			}
			
			else
			{
				
				bool ok = false;
				
				// excessao necessaria pra thief entrar
				//enteringStealHole = false;
				bool steal_this_tile = false;
				if (currentPowerup == POWERUP::ROBBER && goingToUseAbility > 0 && this->path.size() == 1)
				{
					if (AntHill * targetAnthill = gameManager->get_AntHill_at_tile(nextTile.x, nextTile.y) )
					{
						if (targetAnthill->owner != this->owner)
						{
							enteringStealHole = true;
							steal_this_tile = true;
						}
					}
				}
				
				if (!steal_this_tile)
				{
					IntVector2 pos = this->get_tile_position();
					if ( !gameManager->gameMap.tile_is_solid(pos.x, pos.y) )
						enteringStealHole = false;
				}
				// ---
				
				
				
				// surgiu algo no caminho?
				if (!enteringStealHole && !gameManager->tile_is_walkable(nextTile.x, nextTile.y, get_movement_type() ) || gameManager->tile_has_landmine(nextTile.x, nextTile.y, get_owner_index() ) )
					ok = recalculate_path();
				else
					ok = true;
				
				if (ok)
				{
					
					Tile * old_tile = get_tile();
					const int8_t old_tile_type = old_tile ? old_tile->type : 0;
					const int8_t old_tile_secondary = old_tile ? old_tile->secondary : 0;
					const bool old_water = (old_tile_type == TILE::WATER && old_tile_secondary != SECONDARY_TILE::MUD_BRIDGE);
					//const int8_t old_tile_type = get_tile_type();
					
					Vector2 tile_center { (float)(nextTile.x * TILE_SIZE + TILE_SIZE / 2), (float)(nextTile.y * TILE_SIZE + TILE_SIZE / 2) };
					this->angle = angle_between_points(this->position.x, this->position.y, tile_center.x, tile_center.y);
					
					Tile * next_tile = gameManager->gameMap.get_tile(nextTile.x, nextTile.y);
					const int8_t next_tile_type = next_tile ? next_tile->type : 0;
					const int8_t next_tile_secondary = next_tile ? next_tile->secondary : 0;
					const bool next_water = (next_tile_type == TILE::WATER && next_tile_secondary != SECONDARY_TILE::MUD_BRIDGE);
					//const int8_t next_tile_type = gameManager->gameMap.get_tile_type(nextTile.x, nextTile.y);
					
					float speed = WALK_SPEED * (next_tile_type == TILE::MUD || next_tile_secondary == SECONDARY_TILE::MUD_BRIDGE ? 0.5f : 1.0f);
					
					const float rad_angle = degrees_to_radians(this->angle);
					this->position.x += cos(rad_angle) * speed;
					this->position.y += sin(rad_angle) * speed;
					
					moved = true;
					
					// trava para que outras formigas nao consigam entrar no mesmo frame
					if (entering_anthill)
					{
						IHaveTheAnthillLock = true;
						if (myHill) 
							myHill->antLocked = true;
					}
					
					// destrava
					else if (IHaveTheAnthillLock && (myHill && myHill->antLocked) )
					{
						if (!next_over_anthill)
						{
							IHaveTheAnthillLock = false;
							myHill->antLocked = false;
						}
					}
					
					
					// verifica se pulou na agua , a ultima condicao eh para evitar repeticao ate entrar de fato na agua
					if (currentPowerup == POWERUP::DIVER && !old_water && next_water)
					{
						
						Tile * current_tile = get_tile();
						if (current_tile && current_tile->type == TILE::WATER && current_tile->secondary != SECONDARY_TILE::MUD_BRIDGE)
						{
							//printf("entrando na agua %d\n", (int)old_tile_type);
							emit_sound(s_splash);
						}
						
					}
					
					// --- alcancou o tile? tira da fila
					float tolerance = path.size() == 1 ? 2.0f : TILE_SIZE * 0.5f;
					if (gameManager->points_close(this->position.x, this->position.y, tile_center.x, tile_center.y, tolerance) )
						this->path.pop(); 
					
					/*
					this->angle = angle_between_points(this->x, this->y, nextTile.x, nextTile.y);
					this->path.pop();
					this->x = nextTile.x;
					this->y = nextTile.y;
					*/
					
					// checa se chegou no formigueiro 
					//if (AntHill * myHill = get_my_AntHill() )
					if (myHill)
					{
						
						Vector2 anthill_hole_position = myHill->get_hole_position();
						if (gameManager->points_close(this->position.x, this->position.y, anthill_hole_position.x, anthill_hole_position.y, 2.0f) )	
						{
							
							// depositar a comida e ir pegar mais
							if (this->carriedFood) //this->haveFood)
							{
								
								this->position = anthill_hole_position;
								
								actionDelay = 60;
								//this->haveFood = false;
								
								if (owner) owner->foodPoints += carriedFood;
								
								//create_FlyingText( FlyingText(this->x * TILE_SIZE + TILE_SIZE / 2, this->y * TILE_SIZE + TILE_SIZE / 2, "+" + to_stringi(10) ) );
								gameManager->create_FlyingText( FlyingText(this->position.x, this->position.y, "+" + to_stringi(carriedFood), false ) );
								
								if (owner == gameManager->localPlayer)
									emit_sound(s_point);
								
								this->destination = this->targetFoodPosition;
								this->goingAfterFood = true;
								
								this->carriedFood = 0;
								
								gameManager->find_path(this->get_tile_position(), this->destination, this->path, get_movement_type(), false, get_owner_index() );
								
								pushBlocked = true;
								
							}
							
							else if (this->reached_destination() && HP_is_full() ) // so sair do caminho
							{
								move_away(*myHill);
							}
							
						}
					}
					
					
					if (goingAfterFood) //!this->haveFood)
					{
						// checa se colidiu com comida para pegar
						if (Food * current_food = gameManager->get_food_at_radius(this->position.x, this->position.y, TILE_SIZE / 2) )
						{
							
							// confirma se eh a comida desejada // NAO TA FUNCIONANDO
							if (current_food->tile_collision(targetFoodPosition.x, targetFoodPosition.y) )
                            {
                                
                                // nao tem comida? pega
                                if (this->carriedFood <= 0)
                                {
                                    this->harvest(*current_food);
                                    actionDelay = 30;
                                    pushBlocked = true;
                                }
                                
                                // ja tem, muda o alvo e volta pro formigueiro
                                else
                                {
                                    goingAfterFood = false; // impede de ficar presa na comida
                                    targetFoodPosition = current_food->get_tile_position(); 
                                    set_destination_back_to_anthill();
                                }
                                
                            }
                            
                            /*
                            else 
                            {
                                //current_food->debug_printf();
                                IntVector2 food_tile = current_food->get_tile_position();
                                
                                printf("targetFoodPosition: %d, %d; food_tile: %d, %d\n", targetFoodPosition.x, targetFoodPosition.y, food_tile.x, food_tile.y);
                                
                            }*/
                            
						}
						
						// roubo continuo
						else if (currentPowerup == POWERUP::ROBBER && carriedFood <= 0)
                        {
                            
                            IntVector2 myTilePosition = get_tile_position();
                            if (AntHill * targetAntHill = gameManager->get_AntHill_at_tile(myTilePosition.x, myTilePosition.y) )
                            {
                                
                                if (this->owner != targetAntHill->owner)
                                {
                                	
                                	IntVector2 steal_hole_position = targetAntHill->get_steal_hole_tile_position();
                                	if (myTilePosition == steal_hole_position)
									{
										
										if (!attempt_steal_from_anthill(*targetAntHill) )
										{
											// ja tem, muda o alvo e volta pro formigueiro
											goingAfterFood = false; // impede de ficar presa na comida
											//targetFoodPosition = myTilePosition; 
											//set_destination_back_to_anthill();
										}
										
									}
                                }
                                
                            }
                        }
					}
					
					
					// checa se colidiu com item
					if (reached_destination() )
					{
						if (Powerup * current_Powerup = gameManager->get_powerup_at_radius(this->position.x, this->position.y, TILE_SIZE * 0.25f) )
						{
							this->get_powerup(*current_Powerup);
							actionDelay = 60;
						}
					}
					
				}
				
				else 
				{
					clear_path();
					printf("Path erased\n");
				}
				
			}
		}
		
		else // chegou no fim
		{
			
			// aplica habilidades especiais aqui
			if (goingToUseAbility)
			{
				
				int8_t option = goingToUseAbility;
				goingToUseAbility = false;
				
				// checagem de distancia
				IntVector2 tile_position = get_tile_position();
				if (tile_distance(tile_position.x, tile_position.y, abilityPosition.x, abilityPosition.y) <= 1)
                {
                    
                    // ativar habilidade aqui
                    if (currentPowerup == POWERUP::FIREMAN)
                    {
                        
                        center_at_tile();
                        angle = angle_between_points( (int)(position.x / TILE_SIZE), (int)(position.y / TILE_SIZE), abilityPosition.x, abilityPosition.y);
                        //printf("position: %d, %d; angle %.2f\n", abilityPosition.x, abilityPosition.y, angle);
                        
                        if (option == 1)
                        {
                            gameManager->gameMap.set_fire(abilityPosition.x, abilityPosition.y, 127);
                            emit_sound(s_create_fire);
                        }
                        else
                        {
                            gameManager->gameMap.set_fire(abilityPosition.x, abilityPosition.y, 0);
                            emit_sound(s_extinguish_fire);
                        }
                        
                        actionDelay = 60;
                        
                    }
                    
                    else if (currentPowerup == POWERUP::BOMBER)
                    {
                        
                        center_at_tile();
                        angle = angle_between_points( (int)(position.x / TILE_SIZE), (int)(position.y / TILE_SIZE), abilityPosition.x, abilityPosition.y);
                        
                        if (option == 1)
                        {
                            gameManager->gameMap.deploy_land_mine(abilityPosition.x, abilityPosition.y, owner ? owner->index : 0);
                            emit_sound(s_lay_mine);
                        }
                        else
                        {
                            gameManager->gameMap.remove_land_mine(abilityPosition.x, abilityPosition.y);
                            emit_sound(s_small_explosion);
                        }
                        
                        actionDelay = 60;
                        
                    }
                    
                    else if (currentPowerup == POWERUP::DIVER)
                    {
                        
                        center_at_tile();
                        angle = angle_between_points( (int)(position.x / TILE_SIZE), (int)(position.y / TILE_SIZE), abilityPosition.x, abilityPosition.y);
                        
                        if (option == 1)
                        {
                            gameManager->gameMap.set_mud_bridge(abilityPosition.x, abilityPosition.y, 127);
                            
                        }
                        else
                        {
                            gameManager->gameMap.set_mud_bridge(abilityPosition.x, abilityPosition.y, 0);
                            
                        }
                        
                        //emit_sound(s_mud_splash);
                        actionDelay = 60;
                        
                    }
                    
                    else if (currentPowerup == POWERUP::ROBBER)
                    {
                        
                        if (carriedFood <= 0)
                        {
                            
                            IntVector2 myTilePosition = get_tile_position();
                            
                            if (AntHill * targetAntHill = gameManager->get_AntHill_at_tile(myTilePosition.x, myTilePosition.y) )
                            {
                            	if (this->owner != targetAntHill->owner)
                                {
                                	
                                	IntVector2 steal_hole_position = targetAntHill->get_steal_hole_tile_position();
                                	if (myTilePosition == steal_hole_position)
									{
										attempt_steal_from_anthill(*targetAntHill);
									}
                                }
                            }
                            
                        }
                        
                    }
				
                }
				
			}
			
			// heal parado no formigueiro
			if (!HP_is_full() )
			{
			
				//if (AntHill * myHill = get_my_AntHill() )
				if (myHill)
				{
					
					Vector2 hole_position = myHill->get_hole_position();
					
					if (gameManager->points_close(this->position.x, this->position.y, hole_position.x, hole_position.y, 2.0f) )	
					{
						
						HP += 1;
						actionDelay = 20;
						
						const float offset = TILE_SIZE * 0.5f;
						const float spawn_x = this->position.x + fRand(-offset, offset);
						const float spawn_y = this->position.y + fRand(-offset, offset);
						gameManager->create_HealParticle( HealParticle(spawn_x, spawn_y) );
						
						// terminou de healar? vaza
						if (HP_is_full() )
							move_away(*myHill);
						
					}
				}
			
			}
			
		}
	
	}
	
	
	// checagem de interacao com o terreno
	if (position_z <= 0.0f)
	{
		
		IntVector2 myTilePosition = get_tile_position();
		if (Tile * tile = gameManager->gameMap.get_tile(myTilePosition.x, myTilePosition.y) )
		{
			
			// caindo na agua
			if (tile->type == TILE::WATER && tile->secondary != SECONDARY_TILE::MUD_BRIDGE)
			{
				
				if (!was_grounded_previous_frame)
				{
					//printf("caiu na agua\n");
					emit_sound(s_splash);
					// TODO: emitir efeito de agua
					
					// drown
					if (currentPowerup != POWERUP::DIVER)
					{
						//printf("afogando\n");
						HP = 0;
						actionDelay = 60;
						//center_at_tile();
						velocity.x = 0.0f;
						velocity.y = 0.0f;
						emit_sound(s_drown);
					}
				}
				
			}
			
			// mina
			else if (tile->has_landmine() ) 
			{
			    
			    const int mineOwnerIndex = tile->secondary - SECONDARY_TILE::LANDMINE_0;
				
				// so explode se for mina de outro jogador, ou se tiver sido explicitamente mandado pra la, ou se foi jogado ali
				if (oldPosition_z > 0.0f || mineOwnerIndex != get_owner_index() || destination == myTilePosition)
                {
                    
                    tile->secondary = SECONDARY_TILE::NONE;
                    
                    HP -= 2;
                    actionDelay = 60;
                    myAttackTarget = NULL;
                    clear_path();
                    
                    center_at_tile();
                    
                    // joga pra longe, falta melhorar a determinacao do angulo
                    add_force(angle + 180.0f, 5.0f, GRAVITY_FORCE * 14.0f);
                    
                    IntVector2 tilePosition = get_tile_position();
                    //gameManager->create_Explosion( Explosion(tilePosition.x, tilePosition.y) );
                    gameManager->create_explosion(tilePosition.x, tilePosition.y);
                    emit_sound(s_explosion);
                    
                }
                
			}
			
			// queima
			else if (tile->secondary == SECONDARY_TILE::FIRE && currentPowerup != POWERUP::FIREMAN)
			{
				
				HP -= 1;
				actionDelay = 20;
				myAttackTarget = NULL;
				myAttackTarget_id = 0;
				clear_path();
				
				center_at_tile();
				
				// joga pra tras
				add_force(angle + 180.0f, 3.0f, GRAVITY_FORCE * 6.0f);
				
			}
			
			// obstaculos
			/*
			else if (tile->type == TILE::PLANT)
			{
				add_force(angle + 180.0f, 4.0f, 0.0f);
			}*/
			
		}
		
	}
	
	update_animation(moved);
	
	// interpolacao do angulo
	if (visualAngle != angle)
	{
		
		float difference = angle - visualAngle;
		if (difference > 180.0f)
			visualAngle += 360.0f;
		
		else if (difference < -180.0f)
			visualAngle -= 360.0f;
		
		visualAngle = visualAngle * (1.0 - ANGLE_INTERPOLATION_RATIO) + angle * ANGLE_INTERPOLATION_RATIO;
		
		if ( fabs(angle - visualAngle) <= 0.1f)
			visualAngle = angle;
		
	}
	
}


void Ant::update_animation(bool moved)
{
	
	// seta animacao de andar se nao tiver nada
	if (animationType == ANIMATION_TYPE::NONE && moved)
	{
		animationType = ANIMATION_TYPE::MOVING;
		// todo: voltar pro primeiro frame e setar o tempo
	}
	
	else if (animationType == ANIMATION_TYPE::MOVING && !moved)
	{
		//animationType = ANIMATION_TYPE::IDLE;
		animationType = ANIMATION_TYPE::NONE;
	}
	
	
	if (animationType != ANIMATION_TYPE::NONE)
	{
		
		animationTime--;
		if (animationTime <= 0)
		{
			
			int totalAnimationFrames = 1;
			bool loopAnimation = false;
			
			switch(animationType)
			{
				
				case ANIMATION_TYPE::IDLE:
				{
					animationTime = 10;
					totalAnimationFrames = ANT_ANIMATION_FRAMES;
					loopAnimation = true;
					break;
				}
				
				case ANIMATION_TYPE::MOVING:
				{
					animationTime = 10;
					totalAnimationFrames = ANT_ANIMATION_FRAMES;
					loopAnimation = true;
					break;
				}
				
				case ANIMATION_TYPE::TRANSFORMING:
				{
					animationTime = 4;
					totalAnimationFrames = 10;
					loopAnimation = false;
					break;
				}
				
			}
			
			animationFrame++;
			if (animationFrame >= totalAnimationFrames)
			{
				animationFrame = 0;
				if (!loopAnimation)
					animationType = ANIMATION_TYPE::NONE;
			}
			
		}
		
	}
	
}



void Ant::set_angle(float angle, bool interpolate)
{
	
	this->angle = angle;
	if (!interpolate)
		this->visualAngle = angle;
	
}



void Ant::harvest(Food & food)
{
	
	//haveFood = true;
	carriedFood = food.get_points_per_piece(); //10;
	carriedFoodColor = food.get_piece_color();
	
	
	food.points--;
	if (food.points <= 0)
		food.destroy();
	
	targetFoodPosition = food.get_tile_position(); 
	set_destination_back_to_anthill();
	
	emit_sound(s_bite);
	
}


void Ant::harvest(FoodChunk & foodChunk)
{
	
	carriedFood = foodChunk.points;
	carriedFoodColor = foodChunk.color;
	
	foodChunk.destroy();
	
	targetFoodPosition = foodChunk.position; //food.get_tile_position(); 
	set_destination_back_to_anthill();
	
	emit_sound(s_bite);
	
}



void Ant::get_powerup(Powerup & powerup)
{
	
	center_at_tile();
	
	// dropa o powerup atual se tiver
	if (currentPowerup != POWERUP::NONE)
	{
		IntVector2 tile_position = get_tile_position();
		gameManager->create_powerup_around_tile(tile_position.x, tile_position.y, currentPowerup, false);
		//gameManager->create_Powerup( Powerup(tile_position.x + 1, tile_position.y, currentPowerup) );
	}
	
	currentPowerup = powerup.type;
	//printf("%d\n", powerup.type);
	
	// cancela possiveis acoes bufferizadas
	goingToUseAbility = false;
	
	animationType = ANIMATION_TYPE::TRANSFORMING;
	animationFrame = 0;
	animationTime = 4;
	
	emit_sound(s_get_powerup);
	
	powerup.destroy();
	
}



void Ant::hit(Ant & target)
{
	
	float angle_to_rad = angle_between_points_radians(this->position.x, this->position.y, target.position.x, target.position.y);
	
	this->actionDelay = 40;
	this->myAttackTarget = NULL;
	this->clear_path();
	this->angle = radians_to_degrees(angle_to_rad);
	
	const bool strong = this->currentPowerup == POWERUP::STRENGTH;
	
	// olha pro alvo
	target.angle = this->angle + 180.0f; //angle_between_points(target.x, target.y, this->x, this->y);
	if (target.angle > 360.0f)
		target.angle -= 360.0f;
	
	target.HP -= (strong ? 2 : 1);
	target.actionDelay = 60;
	target.myAttackTarget = NULL;
	target.myAttackTarget_id = 0;
	target.clear_path();
	
	// desloca
	const float force = strong ? 6.0f : 3.0f;
	const float z_force = GRAVITY_FORCE * 6.0f * (strong ? 1.5f : 1.0f);
	target.add_force_rad(angle_to_rad, force, z_force);
	
	
	
	/*
	const float distance = TILE_SIZE * (strong ? 4.0f : 1.0f);
	target.position.x += cos(angle_to_rad) * distance;
	target.position.y += sin(angle_to_rad) * distance;
	target.center_at_tile();
	*/
	
	
	emit_sound(s_punch);
	
	/*
	// drown
	if (target.get_tile_type() == TILE::WATER)
	{
		target.HP = 0;
		emit_sound(s_splash);
		emit_sound(s_drown);
	}*/
	
}



void Ant::die()
{
	
	// dropa powerup
	if (currentPowerup != POWERUP::NONE)
	{
		IntVector2 tile_position = get_tile_position();
		//gameManager->create_Powerup( Powerup(tile_position.x, tile_position.y, currentPowerup) );
		gameManager->create_powerup_around_tile(tile_position.x, tile_position.y, currentPowerup, true);
	}
	
	// dropa comida
	if (carriedFood > 0)
	{
		gameManager->create_FoodChunk( FoodChunk(position.x, position.y, carriedFood, carriedFoodColor) );
	}
	
	if (IHaveTheAnthillLock)
		IHaveTheAnthillLock = false;
		
	if (owner)
		owner->deaths++;
	
	
	if (!underwater() )
	{
		gameManager->create_death_smoke(position.x, position.y);
		play_sound(s_puff);
	}
	
	destroy();
	
}



bool Ant::steal_food(AntHill & targetAntHill, int points)
{
    
    const int food_to_rob = std::min(targetAntHill.owner->foodPoints, points);
    
    if (food_to_rob > 0)
    {
        //haveFood = food_to_rob;
        carriedFood = food_to_rob;
        carriedFoodColor = WHITE;
        targetAntHill.owner->foodPoints -= food_to_rob;
        
        gameManager->create_FlyingText( FlyingText(targetAntHill.position.x, targetAntHill.position.y, "-" + to_stringi(food_to_rob), true) );
        
        return true;
    }
    
    return false;
    
}


bool Ant::attempt_steal_from_anthill(AntHill & targetAntHill)
{
    
    center_at_tile();
    actionDelay = 60;
    
    if (targetAntHill.owner && targetAntHill.owner->foodPoints > 0)
    {
        
        if (steal_food(targetAntHill, 50) )
        {
        
            targetFoodPosition = get_tile_position();
            set_destination_back_to_anthill();
            
            emit_sound(s_steal);
            
            // alerta o dono
            if (targetAntHill.owner->is_local_player() )
                play_sound(s_thiefAlert);
            
            return true;
            
        }
        
    }
    
    else 
    {
        if (owned_by_local_player() )
            play_sound(s_no_no);
    }
    
    return false;
    
}



void Ant::add_force(float angle, float force, float z_force)
{
	add_force_rad( degrees_to_radians(angle), force, z_force);
}



void Ant::add_force_rad(float rad_angle, float force, float z_force)
{
	
	velocity.x = cos(rad_angle) * force;
	velocity.y = sin(rad_angle) * force;
	velocity.z = z_force;
	
	if (z_force > 0.0f)
		position_z = 0.1f; // impede de comecar grounded
	
	// impede de manter travado
	if (IHaveTheAnthillLock)
		IHaveTheAnthillLock = false;
	
}



void Ant::set_destination_back_to_anthill()
{
	
	//AntHill * myHill = &gameManager->antHill;
	if (AntHill * myHill = get_my_AntHill() )
	{
		
		//this->destination.x = myHill->x;
		//this->destination.y = myHill->y;
		this->destination = myHill->get_hole_tile_position();
		
		gameManager->find_path( this->get_tile_position(), this->destination, this->path, get_movement_type(), false, get_owner_index() );
		
	}
	
}


bool Ant::recalculate_path()
{
	return gameManager->find_path(this->get_tile_position(), this->destination, this->path, get_movement_type(), goingToUseAbility, get_owner_index() );
}



void Ant::move_away()
{
	
	// nao pode usar rand, arrombado
	//this->destination = IntVector2(tile_position.x + iRand(-2, 2), tile_position.y + iRand(-2, 2) );
	
	const float distance = TILE_SIZE * 3.0f;
	
	//const int seed = (owner ? owner->foodPoints : 0) + (int)angle + unique_id;
	const int seed = gameManager->gameTime + (int)angle * (unique_id + 1);
	//printf("%d ", seed);
	float target_angle = (float)clamp_wrap(seed, 0, 360);
	
	for (int i = 0; i < 16; i++)
    {
        
        const float rad_angle = degrees_to_radians(target_angle);
        
        const float x = position.x + cos(rad_angle) * distance;
        const float y = position.y + sin(rad_angle) * distance;
        
        this->destination = IntVector2( (int)(x / (float)TILE_SIZE), (int)(y / (float)TILE_SIZE) );
        IntVector2 tile_position = this->get_tile_position();
        
        bool success = gameManager->find_path( tile_position, this->destination, this->path, get_movement_type(), false, get_owner_index() );
        if (success)
            break;
        else
        {
            target_angle += 22.5f;
            //printf("move_away() failed, trying new angle\n");
        }
        
    }
    
}


void Ant::move_away(AntHill & targetAnthill)
{
	
	if (targetAnthill.is_symmetrical() )
		move_away();
	else
	{
		
		IntVector2 hillTile = targetAnthill.get_tile_position();
		this->destination = IntVector2( hillTile.x + 4, hillTile.y + 4);
        IntVector2 tile_position = this->get_tile_position();
        
        bool success = gameManager->find_path( tile_position, this->destination, this->path, this->get_movement_type(), false, get_owner_index() );
		if (!success)
			printf("move_away blocked");
	}
	
}



void Ant::clear_path()
{
	while (!path.empty() )
        path.pop();
}



AntHill * Ant::get_my_AntHill() const
{
	return gameManager->get_AntHill(owner);
}


ColorRGBA Ant::get_my_Color(float alpha) const
{
	//return (owner ? ColorRGBA(owner->color.r * alpha, owner->color.g * alpha, owner->color.b * alpha, alpha) : ColorRGBA(alpha, alpha, alpha, alpha) );
	if (owner)
	{
		ColorRGBA base_color = owner->get_color();
		return ColorRGBA(base_color.r * alpha, base_color.g * alpha, base_color.b * alpha, alpha);
	}
	return ColorRGBA(alpha, alpha, alpha, alpha);

}



ColorRGBA Ant::get_my_bright_Color() const
{
	
	if (owner)
		return owner->get_bright_color();
	return WHITE;

}


void Ant::draw() const
{
	
	// caso especial
	if (animationType == ANIMATION_TYPE::TRANSFORMING)
	{
		
		Bitmap & sprite = AntTransformationSprite;
		
		const float src_width = sprite.get_width() / 5;
		const float src_height = sprite.get_height(); // / 2;
		
		const int frameIndex = animationFrame < 5 ? animationFrame : 9 - animationFrame;
		const float src_x = frameIndex * src_width;
		
		const float draw_width = src_width;
		const float draw_height = src_height;
		
		const float half_width = draw_width * 0.5f;
		const float half_height = draw_height * 0.5f;
		
		const float draw_x = position.x - half_width;
		const float draw_y = position.y - half_height - position_z * 0.5f;
		
		// tint part
		spriteCache.draw_tinted_sprite_region(sprite, get_my_Color(1.0), src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);
		
	}
	
	else
	{
	
		Bitmap * sprite;
		if (currentPowerup == POWERUP::STRENGTH)
			sprite = &antSoldierSprite;
		
		else if (currentPowerup == POWERUP::FIREMAN)
			sprite = &antFiremanSprite;
		
		else if (currentPowerup == POWERUP::DIVER)
			sprite = &antDiverSprite;
		
		else if (currentPowerup == POWERUP::ROBBER)
			sprite = &antRobberSprite;
		
		else if (currentPowerup == POWERUP::BOMBER)
			sprite = &antBomberSprite;
		
		else
			sprite = &antSprite;
		
		
		//const float sub_atlas_width = sprite->get_width() / 2;
		
		//const float src_width = sub_atlas_width / ANT_ANIMATION_FRAMES;
		
		const float src_width = sprite->get_width() / ANT_ANIMATION_FRAMES;
		const float src_height = sprite->get_height() / 2;
		
		//const float src_offset = animationType == ANIMATION_TYPE::MOVING ? sub_atlas_width : 0;
		//const float src_x = src_offset + animationFrame * src_width;
		const float src_x = animationFrame * src_width;
		
		const float draw_width = src_width;
		const float draw_height = src_height;
		
		const float half_width = draw_width * 0.5f;
		const float half_height = draw_height * 0.5f;
		
		const float draw_x = position.x; //(float)this->x * TILE_SIZE + TILE_SIZE * 0.5f;
		const float draw_y = position.y - position_z * 0.5f; //(float)this->y * TILE_SIZE + TILE_SIZE * 0.5f;
		
		
		float alpha = underwater() ? 0.5f : 1.0f;
		if (show_debug && IHaveTheAnthillLock)
			alpha = 0.78f;
		
		const float rad_angle = degrees_to_radians(this->visualAngle);
		
		// tint part
		spriteCache.draw_tinted_rotated_sprite_region(*sprite, get_my_Color(alpha), half_width, half_height, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height, rad_angle);
		
		// fixed color part
		spriteCache.draw_tinted_rotated_sprite_region(*sprite, ColorRGBA(alpha, alpha, alpha, alpha), half_width, half_height, src_x, src_height, src_width, src_height, draw_x, draw_y, draw_width, draw_height, rad_angle);
		
	}
	
}


void Ant::draw_food() const
{
	
	//if (haveFood)
	if (carriedFood && animationType != ANIMATION_TYPE::TRANSFORMING)
	{
		
		const float distance = 15.0f;
		const float rad_angle = degrees_to_radians(visualAngle);
		
		const float food_src_width = foodPieceSprite.get_width();
		const float food_src_height = foodPieceSprite.get_height();
		
		const float food_draw_width = food_src_width;
        const float food_draw_height = food_src_height;
        
		const float food_draw_x = position.x + cos(rad_angle) * distance - food_draw_width * 0.5f;
		const float food_draw_y = position.y - position_z * 0.5f + sin(rad_angle) * distance - food_draw_height * 0.5f;
		
		ColorRGBA tint = underwater() ? change_color_opacity(carriedFoodColor, 0.5f) : carriedFoodColor;
		
		spriteCache.draw_tinted_sprite_region(foodPieceSprite, tint, 0.0f, 0.0f, food_src_width, food_src_height, food_draw_x, food_draw_y, food_draw_width, food_draw_height);
		
	}
	
}



void Ant::draw_shadow() const
{
	
	if (position_z <= 0.0f || animationType == ANIMATION_TYPE::TRANSFORMING) return;
	
	Bitmap * sprite;
	if (currentPowerup == POWERUP::STRENGTH)
		sprite = &antSoldierSprite;
	
	else if (currentPowerup == POWERUP::FIREMAN)
		sprite = &antFiremanSprite;
	
	else if (currentPowerup == POWERUP::DIVER)
		sprite = &antDiverSprite;
	
	else if (currentPowerup == POWERUP::ROBBER)
		sprite = &antRobberSprite;
	
	else if (currentPowerup == POWERUP::BOMBER)
		sprite = &antBomberSprite;
	
	else
		sprite = &antSprite;
	
	
	const float src_width = sprite->get_width() / ANT_ANIMATION_FRAMES;
	const float src_height = sprite->get_height() / 2;
	const float src_x = animationFrame * src_width;
	
	const float draw_width = src_width;
    const float draw_height = src_height;
    
	const float half_width = draw_width * 0.5f;
    const float half_height = draw_height * 0.5f;
	
	const float draw_x = position.x; 
	const float draw_y = position.y; 
	
	const float rad_angle = degrees_to_radians(this->visualAngle);
	
	//DrawTexturePro(*sprite, Rectangle { src_x, 0.0f, src_width, src_height }, Rectangle { draw_x, draw_y, src_width, src_height }, Vector2 { half_width, half_height }, this->visualAngle, ColorRGBA { 0, 0, 0, 128} );
	
	spriteCache.draw_tinted_rotated_sprite_region(*sprite, ColorRGBA(0.0f, 0.0f, 0.0f, 0.5f), half_width, half_height, src_x, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height, rad_angle);
	
}



void Ant::draw_path() const
{
	
	if (path.empty() ) return;
	
	std::queue<IntVector2> pathCopy = path;
	
	Vector2 cursorStart = this->position;
	Vector2 cursorEnd;
	
	//unsigned char color = 255;
	//bool color_direction = false;
	
	//spriteCache.set_texture(whiteSquareBitmap);
	
	const float line_width = 1.0f / gameManager->camera.zoom;
	
	while (!pathCopy.empty() )
	{
		
		IntVector2 & current = pathCopy.front();
		
		cursorEnd.x = current.x * TILE_SIZE + TILE_SIZE * 0.5f;
		cursorEnd.y = current.y * TILE_SIZE + TILE_SIZE * 0.5f;
		
		//DrawLine(cursorStart.x, cursorStart.y, cursorEnd.x, cursorEnd.y, ColorRGBA { color, color, color, 255} );
		spriteCache.draw_line(cursorStart.x, cursorStart.y, cursorEnd.x, cursorEnd.y, WHITE, line_width);
		
		
		cursorStart = cursorEnd;
		
		pathCopy.pop();
		
		//if (color == 255)
		//	color -= 50;
		
		/*
		if (color_direction == true)
		{
			color += 50;
			if (color >= 255)
			{
				color = 255;
				color_direction = !color_direction;
			}
		}
		else
		{
			color -= 50;
			if (color <= 0)
			{
				color = 0;
				color_direction = !color_direction;
			}
		}*/
	}
	
}



bool Ant::moving() const
{
	return !path.empty();
}



bool Ant::reached_destination() const
{
	return path.empty() || get_tile_position() == destination; //(x == destination.x && y == destination.y);
}



bool Ant::underwater() const
{
	
	if (position_z <= 0.0f)
	{
		if (Tile * tile = get_tile() )
			return (tile->type == TILE::WATER && tile->secondary != SECONDARY_TILE::MUD_BRIDGE);
	}
	return false;
	
}



bool Ant::is_enemy(Ant & other) const
{
	return this->owner != other.owner;
}



bool Ant::in_attack_range(Ant & other) const
{
	
	const float range = (TILE_SIZE + 8);
	const float distance_squared = distance_between_points_squared(this->position.x, this->position.y, other.position.x, other.position.y);
	return (distance_squared <= range * range);
	
}



bool Ant::have_a_field_special_ability()
{
	return (currentPowerup == POWERUP::FIREMAN || currentPowerup == POWERUP::DIVER || currentPowerup == POWERUP::ROBBER || currentPowerup == POWERUP::BOMBER);
	
}


int8_t Ant::get_movement_type() const
{
	
	if (currentPowerup == POWERUP::FIREMAN)
		return MOVEMENT_TYPE::FIRE;
	
	else if (currentPowerup == POWERUP::DIVER)
		return MOVEMENT_TYPE::SWIM;
	
	//else if (currentPowerup == POWERUP::ROBBER)
	//	return MOVEMENT_TYPE::THIEF;
	
	return MOVEMENT_TYPE::NORMAL;
	
	
}



bool Ant::owned_by_local_player() const
{
	return (owner ? owner->controller == CONTROLLER::LOCAL : false);
}


bool Ant::elegible_for_collisions() const
{
	return position_z <= 0 && enteringStealHole == false;
}



bool Ant::can_collide_with(Ant & other) const
{
	return (this->position_z <= 0.0f && other.position_z <= 0.0f && this->underwater() == other.underwater() );
}



bool Ant::both_wants_to_go_to_the_same_position(Ant & other) const
{
	
	if (this->path.size() == 1 && other.path.size() == 1)
	{
		if (this->path.front() == other.path.front() && !this->busy() && !other.busy() )
			return true;
	}
	return false;
	
}


bool Ant::can_be_hit() const
{
	return (HP > 0 && position_z <= 0.0f);
}


bool Ant::busy() const
{
	return (goingAfterFood || goingToUseAbility || myAttackTarget);
}


bool Ant::idle() const
{
	return (actionDelay <= 0 && reached_destination() );
}
	


uint8_t Ant::get_owner_index() const
{ 
    return owner ? owner->index : 0; 
}


void Ant::emit_sound(Sound & sound)
{
	gameManager->play_area_sound(sound, position.x, position.y);
}


// net
void Ant::pack_data(BitStream & data)
{
    
    GameObject::pack_data(data);
    
    data.addFloat(position_z);
    //data.addFloat(oldPosition_z);
    
    //data.addFloat(oldPosition.x);
    //data.addFloat(oldPosition.y);
    
    data.addFloat(velocity.x);
    data.addFloat(velocity.y);
    data.addFloat(velocity.z);
    
    data.addBool(pushBlocked);
    data.addBool(IHaveTheAnthillLock);
    
    data.addFloat(angle);
    //data.addFloat(visualAngle);
    
    data.addInt16(carriedFood);
    
    /*
    data.addInt(carriedFoodColor.r, 1);
    data.addInt(carriedFoodColor.g, 1);
    data.addInt(carriedFoodColor.b, 1);
    data.addInt(carriedFoodColor.a, 1);
    */
    
    data.addBool(goingAfterFood);
    
    data.addInt8(goingToUseAbility);
    data.addInt8(HP);
    data.addInt8(currentPowerup);
    //data.addInt8(animationFrame);
    //data.addInt8(animationTime);
    data.addInt16(actionDelay);
    data.addInt16(unique_id);
    
    //data.addInt8(owner ? owner->id : 0);
    //data.addInt16(myAttackTarget ? myAttackTarget->unique_id : 0);
    data.addInt8(owner_id);
    data.addInt16(myAttackTarget_id);
    
    data.addInt(destination.x, 2);
    data.addInt(destination.y, 2);
    
    data.addInt(targetFoodPosition.x, 2);
    data.addInt(targetFoodPosition.y, 2);
    
    data.addInt(abilityPosition.x, 2);
    data.addInt(abilityPosition.y, 2);
    
    
    data.addUInt(path.size(), 2);
    if (!path.empty() )
    {
        
        std::queue<IntVector2> path_copy = path;
        
        while (!path_copy.empty() )
        {
            IntVector2 current = path_copy.front();
            data.addInt(current.x, 2);
            data.addInt(current.y, 2);
            path_copy.pop();
        }
        
    }
    
}


void Ant::unpack_data(BitStream & _data)
{
    
    GameObject::unpack_data(_data);
    
    position_z = _data.getFloat();
    oldPosition_z = position_z;
    
    oldPosition = position;
    
    velocity.x = _data.getFloat();
    velocity.y = _data.getFloat();
    velocity.z = _data.getFloat();
    
    pushBlocked = _data.getBool();
    IHaveTheAnthillLock = _data.getBool();
    
    angle = _data.getFloat();
    visualAngle = angle;
    
    carriedFood = _data.getInt16();
    
    goingAfterFood = _data.getBool();
    
    goingToUseAbility = _data.getInt8();
    HP = _data.getInt8();
    currentPowerup = _data.getInt8();
    //animationFrame = _data.getInt8();
    //animationTime = _data.getInt8();
    actionDelay = _data.getInt16();
    unique_id = _data.getInt16();
    
    /*
    int8_t owner_id = _data.getInt8();
    int16_t attackTarget_id = _data.getInt16();
    printf("owner_id: %d; attackTarget_id: %d\n", owner_id, attackTarget_id);
    
    owner = reinterpret_cast<Player*>(owner_id);
    myAttackTarget = reinterpret_cast<Ant*>(attackTarget_id);
    */
    owner_id = _data.getInt8();
    myAttackTarget_id = _data.getInt16();
    
    
    destination.x = _data.getInt(2);
    destination.y = _data.getInt(2);
    
    targetFoodPosition.x = _data.getInt(2);
    targetFoodPosition.y = _data.getInt(2);
    
    abilityPosition.x = _data.getInt(2);
    abilityPosition.y = _data.getInt(2);
    
    //path.clear();
    while(!path.empty() )
        path.pop();
    
    size_t path_size = _data.getUInt(2);
    if (path_size <= 10000)
    {
        for (size_t i = 0; i < path_size; i++)
        {
            int x = _data.getInt(2);
            int y = _data.getInt(2);
            path.push( IntVector2(x, y) );
        }
    }
    else printf("Warning: too much path elements: %d\n", (int)path_size);
    
}


void Ant::refresh_pointers()
{
    
    owner = owner_id != 0 ? gameManager->get_Player_by_id(owner_id) : NULL;
    myAttackTarget = myAttackTarget_id != 0 ? gameManager->get_Ant(myAttackTarget_id) : NULL;
    
    /*
    // interpreta o valor armazenado nos ponteiros como id
    if (owner != NULL)
        owner = gameManager->get_Player_by_id( *(reinterpret_cast<int*>(owner) ) );
    
    if (myAttackTarget != NULL)
        myAttackTarget = gameManager->get_Ant( *(reinterpret_cast<int*>(myAttackTarget) ) );
    */
    
}



void Ant::debug_printf()
{
    
    printf("position: %.2f, %.2f, %.2f\n", position.x, position.y, position_z);
    //Vector2 oldPosition;
    printf("velocity: %.2f, %.2f, %.2f\n", velocity.x, velocity.y, velocity.z);
    printf("pushBlocked: %d\n", pushBlocked ? 1 : 0);
    printf("IHaveTheAnthillLock: %d\n", IHaveTheAnthillLock ? 1 : 0);
    printf("angle: %.2f\n", angle);
    printf("visualAngle: %.2f\n", visualAngle);
    printf("carriedFood: %d\n", carriedFood);
    printf("carriedFoodColor(0-255): %d, %d, %d, %d\n", (int)(carriedFoodColor.r * 255.0f), (int)(carriedFoodColor.g * 255.0f), (int)(carriedFoodColor.b * 255.0f), (int)(carriedFoodColor.a * 255.0f) );
    printf("goingAfterFood: %d\n", goingAfterFood ? 1 : 0);
    printf("goingToUseAbility: %d\n", goingToUseAbility);
    printf("HP: %d\n", HP);
    printf("currentPowerup: %d\n", currentPowerup);
    printf("animationFrame: %d\n", animationFrame);
    printf("animationTime: %d\n", animationTime);
    printf("actionDelay: %d\n", actionDelay);
    printf("unique_id: %d\n", unique_id);
    printf("owner_id: %d\n", owner_id);
    printf("myAttackTarget_id: %d\n", myAttackTarget_id);
    printf("destination: %d, %d\n", destination.x, destination.y);
    printf("targetFoodPosition: %d, %d\n", targetFoodPosition.x, targetFoodPosition.y);
    printf("abilityPosition: %d, %d\n", abilityPosition.x, abilityPosition.y);
    
    printf("path: ");
    std::queue<IntVector2> path_copy = path;
    while (!path_copy.empty() )
    {
        IntVector2 current = path_copy.front();
        printf("(%d, %d), ", current.x, current.y);
        path_copy.pop();
    }
    printf("\n");
    
}
