#include "GameManager.h"


Ant * GameManager::create_Ant(const Ant & O)
{
	
	for (Ant & current : ant) 
	{
		
		if (!current.active)
		{
			current = O;
			return &current;
		}
		
	}
	
	ant.push_back(O);
	return &ant.back();
	
}



FlyingText * GameManager::create_FlyingText(const FlyingText & O)
{
	
	for (FlyingText & current : flyingText) 
	{
		
		if (!current.active)
		{
			current = O;
			return &current;
		}
		
	}
	
	flyingText.push_back(O);
	return &flyingText.back();
	
}



HealParticle * GameManager::create_HealParticle(const HealParticle & O)
{
	
	for (HealParticle & current : healParticle) 
	{
		
		if (!current.active)
		{
			current = O;
			return &current;
		}
		
	}
	
	healParticle.push_back(O);
	return &healParticle.back();
	
}



Powerup * GameManager::create_Powerup(const Powerup & O)
{
	
	for (Powerup & current : powerup) 
	{
		
		if (!current.active)
		{
			current = O;
			return &current;
		}
		
	}
	
	powerup.push_back(O);
	return &powerup.back();
	
	
}


/*
Explosion * GameManager::create_Explosion(const Explosion & O)
{
	
	for (Explosion & current : explosion) 
	{
		
		if (!current.active)
		{
			current = O;
			return &current;
		}
		
	}
	
	explosion.push_back(O);
	return &explosion.back();
	
}
*/


FoodChunk * GameManager::create_FoodChunk(const FoodChunk & O)
{
	
	for (FoodChunk & current : foodChunk) 
	{
		
		if (!current.active)
		{
			current = O;
			return &current;
		}
		
	}
	
	foodChunk.push_back(O);
	return &foodChunk.back();
	
}



AnimatedSprite * GameManager::create_explosion(int x, int y)
{
	
	for (AnimatedSprite & current : animatedSprite) 
	{
		
		if (!current.active)
		{
			current = AnimatedSprite(x, y, ANIMATED_SPRITE::EXPLOSION);
			return &current;
		}
		
	}
	
	animatedSprite.push_back( AnimatedSprite(x, y, ANIMATED_SPRITE::EXPLOSION) );
	return &animatedSprite.back();
	
}


AnimatedSprite * GameManager::create_explosion(float x, float y)
{
	
	for (AnimatedSprite & current : animatedSprite) 
	{
		
		if (!current.active)
		{
			current = AnimatedSprite(x, y, ANIMATED_SPRITE::EXPLOSION);
			return &current;
		}
		
	}
	
	animatedSprite.push_back( AnimatedSprite(x, y, ANIMATED_SPRITE::EXPLOSION) );
	return &animatedSprite.back();
	
}


AnimatedSprite * GameManager::create_death_smoke(float x, float y)
{
	
	for (AnimatedSprite & current : animatedSprite) 
	{
		
		if (!current.active)
		{
			current = AnimatedSprite(x, y, ANIMATED_SPRITE::DEATH_SMOKE);
			return &current;
		}
		
	}
	
	animatedSprite.push_back( AnimatedSprite(x, y, ANIMATED_SPRITE::DEATH_SMOKE) );
	return &animatedSprite.back();
	
}
