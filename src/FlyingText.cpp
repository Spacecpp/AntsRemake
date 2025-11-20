#include "FlyingText.h"
#include "globals.h"
#include "Alegrinho.h"


FlyingText::FlyingText(float x, float y, const std::string & text, bool goDown): GameObject(x, y), goingDown(goDown), lifetime(80), text(text)
{
	
}



void FlyingText::update()
{
	
	if (goingDown)
		position.y += 1.0f;
	else
		position.y -= 1.0f;
	
	
	lifetime--;
	if (lifetime <= 0)
		destroy();
	
}



void FlyingText::draw() const
{
	
	//spriteCache.draw_text_height(the_font, text, WHITE, position.x, position.y, 32.0f, TEXT_ALIGN_CENTER);
	const float scale = the_font.get_scale_for_height(32.0f);
	spriteCache.draw_scaled_text_shadow(the_font, text, WHITE, BLACK, position.x, position.y, scale, scale, TEXT_ALIGN_CENTER);
}
