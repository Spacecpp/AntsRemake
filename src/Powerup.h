#ifndef POWERUP_H
#define POWERUP_H

#include <stdint.h>
#include "GameObject.h"

class BitStream;

namespace POWERUP { enum { NONE, STRENGTH, FIREMAN, ROBBER, DIVER, BOMBER, COUNT }; }

class Powerup: public GameObject
{
	
	public:
		int8_t type;
		
		Powerup();
		Powerup(int x, int y, int8_t type);
		
		//void update();
		
		void draw() const;
		
		// net
		void pack_data(BitStream & data);
		void unpack_data(BitStream & _data);
		
};
#endif
