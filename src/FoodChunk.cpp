#include "FoodChunk.h"

extern Bitmap foodPieceSprite;
extern RenderCache spriteCache;

FoodChunk::FoodChunk(float x, float y, int points, const ColorRGBA & color): GameObject(x, y), points(points), color(color)
{
    
}


void FoodChunk::draw() const
{
    
    const float src_width = foodPieceSprite.get_width();
    const float src_height = foodPieceSprite.get_height();
    
    const float draw_width = src_width;
    const float draw_height = src_height;
    
    const float draw_x = position.x - draw_width * 0.5f;
    const float draw_y = position.y - draw_height * 0.5f;
    
    //DrawTexturePro(foodPieceSprite, Rectangle { 0.0f, 0.0f, src_width, src_height }, Rectangle { draw_x, draw_y, src_width, src_height }, Vector2 { (float)round_to_nearest(src_width * 0.5f), (float)round_to_nearest(src_height * 0.5f) }, 0.0f, color);
    
    spriteCache.draw_tinted_sprite_region(foodPieceSprite, color, 0.0f, 0.0f, src_width, src_height, draw_x, draw_y, draw_width, draw_height);

}
