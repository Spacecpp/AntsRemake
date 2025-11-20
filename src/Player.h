#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "Alegrinho.h"
#include "lists.h"
#include "Colors.h"

class BitStream;

class Player
{
	
	public:
		bool active;
		bool ready, oldReady;
		int8_t id;
		int8_t index;
		int connection_id;
		int8_t controller;
		int foodPoints;
		uint32_t gameFrame;
		std::string name;
		int8_t colorIndex, oldColorIndex;
		//ColorRGBA color;
		
		uint16_t kills, deaths, hatches;
		
		bool netMapOk;
		
		//Player(const std::string & name, const ColorRGBA & color, int8_t controller, int8_t id, int connection_id);
		Player(const std::string & name, int8_t colorIndex, int8_t controller, int8_t id, int connection_id);
		void destroy();
		
		void change_color(int8_t colorIndex);
		void set_ready(bool ready);
		
		/*
		ColorRGBA get_color();
		ColorRGBA get_bright_color();
		*/
		bool disconnected() const;
		
		void pack_resync_data(BitStream & data);
		void unpack_resync_data(BitStream & _data);
		
		void debug_printf();
		
		inline bool is_local_player() const { return controller == CONTROLLER::LOCAL; }
		inline bool is_remote() const { return controller == CONTROLLER::REMOTE || controller == CONTROLLER::REMOTE_AI; }
		
		inline ColorRGBA get_color() { return get_indexed_color(colorIndex); }
		inline ColorRGBA get_bright_color() { return get_indexed_bright_color(colorIndex); }
	
};

bool sortPlayerScore(Player * a, Player * b);

#endif
