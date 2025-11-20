#ifndef GAMEORDER_H
#define GAMEORDER_H

#include <stdint.h>
#include <vector>

class Ant;
class AntHill;
class BitStream;


namespace ORDER_ID { enum { NONE, ANT_MOVE, ATTACK, USE_SPECIAL, EGG_HATCH, COUNT }; }

class GameOrder
{
	
	public:
		uint32_t targetGameFrame;
		int8_t id;
		int8_t playerID;
		int16_t x, y;
		int16_t targetID;
		int8_t option;
		std::vector<int16_t> antUniqueID;
		
		GameOrder();
		void init_ant_move(uint32_t targetGameFrame, std::vector<Ant*> & targetAnts, int16_t x, int16_t y, bool goingAfterFood);
		void init_ant_attack(uint32_t targetGameFrame, std::vector<Ant*> & targetAnts, Ant & defenderAnt);
		void init_ant_use_special_ability(uint32_t targetGameFrame, Ant & targetAnt, int16_t x, int16_t y, int8_t option);
		void init_egg_hatch(uint32_t targetGameFrame, AntHill & targetAntHill);
		
		bool pack(BitStream & data);
		bool unpack(BitStream & _data);
	
};
#endif
