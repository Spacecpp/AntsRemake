#include "HealParticle.h"

extern RenderCache spriteCache;
extern Bitmap healParticleSprite;

HealParticle::HealParticle(float x, float y): GameObject(x, y), lifetime(HEAL_PARTICLE_MAX_LIFETIME)
{
	
}


void HealParticle::update()
{
	
	position.y -= 0.5f;
	
	lifetime--;
	if (lifetime <= 0)
		destroy();
	
}



void HealParticle::draw() const
{
	
	const float opacity = lifetime < HEAL_PARTICLE_MAX_LIFETIME / 2 ? (float)lifetime / (float)(HEAL_PARTICLE_MAX_LIFETIME / 2) : 1.0f;
	ColorRGBA tint(opacity, opacity, opacity, opacity);
	
	const float draw_x = position.x - healParticleSprite.get_width() * 0.5f;
	const float draw_y = position.y - healParticleSprite.get_height() * 0.5f;
	
	spriteCache.draw_tinted_sprite( healParticleSprite, tint, draw_x, draw_y);
	
}
