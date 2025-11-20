#ifndef FOODCHUNK_H
#define FOODCHUNK_H

#include "Alegrinho.h"
#include "GameObject.h"

class FoodChunk: public GameObject
{
    
    protected:
        
    private:
        
    public:
        int16_t points;
        ColorRGBA color;
        
        FoodChunk(float x, float y, int points, const ColorRGBA & color);
        
        void draw() const;
        
        // net
		//void pack_resync_data(BitStream & data);
		//void unpack_resync_data(BitStream & _data);
    
};
#endif
