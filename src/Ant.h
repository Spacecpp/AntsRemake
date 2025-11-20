#ifndef ANT_H
#define ANT_H

#include <queue>
#include <stdint.h>
#include "GameObject.h"
#include "Alegrinho.h"


#define WALK_SPEED 1.5f
#define ANGLE_INTERPOLATION_RATIO 0.15f
#define VELOCITY_DECAY_RATIO 0.25f

namespace MOVEMENT_TYPE { enum { NORMAL, FIRE, SWIM, THIEF }; }
namespace ANIMATION_TYPE { enum { NONE, IDLE, MOVING, TRANSFORMING }; }

class Food;
class FoodChunk;
class Powerup;
class Player;
class AntHill;
class BitStream;


class Ant: public GameObject
{
	
	public:
		float position_z, oldPosition_z;
		Vector2 oldPosition;
		Vector3 velocity;
		bool pushBlocked;
		bool IHaveTheAnthillLock;
		float angle, visualAngle;
		//bool haveFood;
		int16_t carriedFood;
		ColorRGBA carriedFoodColor;
		bool goingAfterFood;
		int8_t goingToUseAbility;
		bool enteringStealHole;
		int8_t HP;
		int8_t currentPowerup;
		int8_t animationFrame;
		int8_t animationTime;
		int8_t animationType;
		int16_t actionDelay;
		int16_t unique_id;
		int8_t owner_id;
		int16_t myAttackTarget_id;
		int8_t groupID;
		Player * owner;
		Ant * myAttackTarget;
		IntVector2 destination;
		IntVector2 targetFoodPosition;
		IntVector2 abilityPosition;
		std::queue<IntVector2> path;
		
		
		Ant();
		Ant(int x, int y, Player * player, int16_t unique_id, int8_t powerup = 0);
		
		void update();
		void update_animation(bool moved);
		
		void set_angle(float angle, bool interpolate);
		
		void harvest(Food & food);
		void harvest(FoodChunk & foodChunk);
		void get_powerup(Powerup & powerup);
		void hit(Ant & target);
		void die();
		bool steal_food(AntHill & targetAntHill, int points);
		bool attempt_steal_from_anthill(AntHill & targetAntHill);
		void add_force(float angle, float force, float z_force);
		void add_force_rad(float rad_angle, float force, float z_force);
		
		void set_destination_back_to_anthill();
		bool recalculate_path();
		void move_away();
		void move_away(AntHill & targetAnthill);
		void clear_path();
		
		AntHill * get_my_AntHill() const;
		ColorRGBA get_my_Color(float alpha = 1.0f) const;
		ColorRGBA get_my_bright_Color() const;
		
		// draw
		void draw() const;
		void draw_food() const;
		void draw_shadow() const;
		void draw_path() const;
		
		bool moving() const;
		bool reached_destination() const;
		bool underwater() const;
		bool is_enemy(Ant & other) const;
		bool in_attack_range(Ant & other) const;
		bool have_a_field_special_ability();
		int8_t get_movement_type() const;
		bool owned_by_local_player() const;
		bool elegible_for_collisions() const;
		bool can_collide_with(Ant & other) const;
		bool both_wants_to_go_to_the_same_position(Ant & other) const;
		bool can_be_hit() const;
		bool busy() const;
		bool idle() const;
		inline bool carrying_food() const { return carriedFood > 0; }
		uint8_t get_owner_index() const;
		
		bool HP_is_full() const { return HP >= 10; }
		
		
		void emit_sound(Sound & sound);
		
		// load
		bool load_food_data();
		
		// net
		void pack_data(BitStream & data);
		void unpack_data(BitStream & _data);
		
		void refresh_pointers();
		
		void debug_printf();
		
};
#endif
