#include "GameOrder.h"
#include "Ant.h"
#include "AntHill.h"
#include "Player.h"
#include "Alegrinho.h"


GameOrder::GameOrder(): targetGameFrame(0), id(0), targetID(0), option(0)
{
	
}


void GameOrder::init_ant_move(uint32_t targetGameFrame, std::vector<Ant*> & targetAnts, int16_t x, int16_t y, bool goingAfterFood)
{
	
	this->targetGameFrame = targetGameFrame;
	this->id = ORDER_ID::ANT_MOVE;
	
	antUniqueID.clear();
	for (Ant * current : targetAnts)
	{
		antUniqueID.push_back(current->unique_id);
	}
	
	this->x = x;
	this->y = y;
	
	this->option = goingAfterFood ? 1 : 0;
	
}



void GameOrder::init_ant_attack(uint32_t targetGameFrame, std::vector<Ant*> & targetAnts, Ant & defenderAnt)
{
	
	this->targetGameFrame = targetGameFrame;
	this->id = ORDER_ID::ATTACK;
	
	antUniqueID.clear();
	for (Ant * current : targetAnts)
	{
		antUniqueID.push_back(current->unique_id);
	}
	
	targetID = defenderAnt.unique_id;
	
}



void GameOrder::init_ant_use_special_ability(uint32_t targetGameFrame, Ant & targetAnt, int16_t x, int16_t y, int8_t option)
{
	
	this->targetGameFrame = targetGameFrame;
	this->id = ORDER_ID::USE_SPECIAL;
	
	antUniqueID.clear();
	antUniqueID.push_back(targetAnt.unique_id);
	
	this->x = x;
	this->y = y;
	
	this->option = option;
	
}



void GameOrder::init_egg_hatch(uint32_t targetGameFrame, AntHill & targetAntHill)
{
	
	this->targetGameFrame = targetGameFrame;
	this->id = ORDER_ID::EGG_HATCH;
	
	this->playerID = targetAntHill.owner ? targetAntHill.owner->id : -1;
	
}



bool GameOrder::pack(BitStream & data)
{
	
	bool error = false;
	
	data.addUInt32(targetGameFrame);
	data.addInt8(id);
	
	switch(id)
	{
		
		case ORDER_ID::ANT_MOVE:
		{
			
			data.addInt(antUniqueID.size(), 2);
			for (int16_t & antID : antUniqueID)
				data.addInt16(antID);
			
			data.addInt16(x);
			data.addInt16(y);
			
			data.addInt8(option);
			
			break;
			
		}
		
		case ORDER_ID::ATTACK:
		{
			
			data.addInt(antUniqueID.size(), 2);
			for (int16_t & antID : antUniqueID)
				data.addInt16(antID);
			
			data.addInt16(targetID);
			
			break;
			
		}
		
		case ORDER_ID::USE_SPECIAL:
		{
			
			int16_t ant_id = 0;
			if (!antUniqueID.empty() )
				ant_id = antUniqueID.front();
			else
				printf("Warning: antUniqueID is empty\n");
			
			data.addInt16(ant_id);
			
			data.addInt16(x);
			data.addInt16(y);
			
			data.addInt8(option);
			
			break;
			
		}
		
		case ORDER_ID::EGG_HATCH:
		{
			data.addInt8(playerID);
			break;
		}
		
		default: 
		{
			error = true;
			printf("Unhandled order id %d\n", id);
		}
		
	}
	
	return !error;
	
}



bool GameOrder::unpack(BitStream & _data)
{
	
	bool error = false;
	
	targetGameFrame = _data.getUInt32();
	id = _data.getInt8();
	
	switch(id)
	{
		
		case ORDER_ID::ANT_MOVE:
		{
			
			int count = _data.getInt(2);
			for (int i = 0; i < count; i++)
			{
				int16_t antID = _data.getInt16();
				antUniqueID.push_back(antID);
			}
			
			x = _data.getInt16();
			y = _data.getInt16();
			
			option = _data.getInt8();
			
			break;
			
		}
		
		case ORDER_ID::ATTACK:
		{
			
			int count = _data.getInt(2);
			for (int i = 0; i < count; i++)
			{
				int16_t antID = _data.getInt16();
				antUniqueID.push_back(antID);
			}
			
			targetID = _data.getInt16();
			
			break;
			
		}
		
		case ORDER_ID::USE_SPECIAL:
		{
			
			int16_t antID = _data.getInt16();
			antUniqueID.push_back(antID);
			
			x = _data.getInt16();
			y = _data.getInt16();
			
			option = _data.getInt8();
			
			break;
			
		}
		
		case ORDER_ID::EGG_HATCH:
		{
			playerID = _data.getInt8();
			break;
		}
		
		default: 
		{
			error = true;
			printf("Unhandled order id %d\n", id);
		}
		
	}
	
	return !error;
	
}
