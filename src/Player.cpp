#include "Player.h"


/*
Player::Player(const std::string & name, const ColorRGBA & color, int8_t controller, int8_t id, int connection_id): 
	active(true), ready(false), id(id), index(0), connection_id(connection_id), controller(controller), foodPoints(0), gameFrame(0), name(name), color(color),
	kills(0), deaths(0), hatches(0)
{
	
}*/

Player::Player(const std::string & name, int8_t colorIndex, int8_t controller, int8_t id, int connection_id): 
	active(true), ready(false), oldReady(ready), id(id), index(0), connection_id(connection_id), controller(controller), foodPoints(0), gameFrame(0), name(name), colorIndex(colorIndex), oldColorIndex(colorIndex),
	kills(0), deaths(0), hatches(0), netMapOk(false)
{
	
}


void Player::destroy()
{
	active = false;
}


void Player::change_color(int8_t colorIndex)
{
	this->colorIndex = colorIndex;
	this->oldColorIndex = colorIndex;
}



void Player::set_ready(bool ready)
{
	this->ready = ready;
	this->oldReady = ready;
}


bool Player::disconnected() const
{
    return (connection_id == NO_PLAYER_CONNECTION_ID);
    
}


/*
ColorRGBA Player::get_color()
{
	return get_indexed_color(colorIndex);
}


ColorRGBA Player::get_bright_color()
{
	return get_indexed_bright_color(colorIndex);
}*/



void Player::pack_resync_data(BitStream & data)
{
    
    data.addInt(foodPoints, 4);
    //kills(0), deaths(0), hatches(0)
    
}


void Player::unpack_resync_data(BitStream & _data)
{
    
    foodPoints = _data.getInt(4);
    
}


void Player::debug_printf()
{
	/*
	bool active;
	bool ready;
	int8_t id;
	int8_t index;
	int connection_id;
	int8_t controller;
	int foodPoints;
	uint32_t gameFrame;
	std::string name;
	int8_t colorIndex;
	
	uint16_t kills, deaths, hatches;
	
	bool netMapOk;
	*/
	
	if (!active) printf("!");
	printf("%s, index %d, id %d\n", name.c_str(), index, id);
	
}


//---
bool sortPlayerScore(Player * a, Player * b)
{
	return a->foodPoints > b->foodPoints;
}
