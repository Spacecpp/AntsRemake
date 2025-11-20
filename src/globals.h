#ifndef GLOBALS_H
#define GLOBALS_H

#include "Alegrinho.h"

extern Shader spriteShader;
extern Shader pointShader;

//extern Image gameIcon;

extern Cursor customCursor;

extern Bitmap whiteSquareBitmap;
extern Bitmap outlinedSquareBitmap;

extern Bitmap antSprite;
extern Bitmap antSoldierSprite;
extern Bitmap antFiremanSprite;
extern Bitmap antRobberSprite;
extern Bitmap antDiverSprite;
extern Bitmap antBomberSprite;

extern Bitmap antFirstFrameSprite[6];


extern Bitmap antHillDefaultSprite[4];

extern Bitmap antHillSymetricSprite;
extern Bitmap antHillFlagSprite;

// terrain
extern Bitmap waterTexture;
extern Bitmap dirtTexture;
extern Bitmap sandTexture;
extern Bitmap rockyGroundTexture;
extern Bitmap mudTexture;
extern Bitmap voidTileTexture;

extern Bitmap mudTransitionTileSprite;
extern Bitmap mudToWaterTransitionTileSprite;
extern Bitmap sandTransitionTileSprite;
extern Bitmap rockyTransitionTileSprite;

extern Bitmap fireSprite;
extern Bitmap landMineSprite;
extern Bitmap explosionSprite;
extern Bitmap mudBridgeSprite;

extern Bitmap terrainAtlas;


extern Bitmap foodPieceSprite;

extern Bitmap powerupAtlas;
extern Bitmap powerupSprite[5];

extern Bitmap powerupDropSprite;

extern Bitmap missingSprite;
extern Bitmap translucentSquaresSprite;
extern Bitmap selectionCircleSprite;

extern Bitmap menuBackgroundTexture;

extern Bitmap HPBarSprite;

extern Bitmap minimapAntSprite;
extern Bitmap minimapAntHillSprite;

extern Bitmap healParticleSprite;

extern Bitmap deathSmokeSprite;
extern Bitmap AntTransformationSprite;

extern Bitmap readyCheckmarkSprite;
extern Bitmap logoSprite;

// GUI
extern Bitmap buttonSprite;
extern Bitmap checkBoxSprite;
extern Bitmap window9sliceSprite;
extern Bitmap window9sliceInsideSprite;
extern Bitmap console9sliceSprite;
extern Bitmap arrowUpSprite;
extern Bitmap arrowDownSprite;
extern Bitmap scrollBoxSprite;
extern Bitmap sliderBarBitmap;
extern Bitmap sliderCursorBitmap;
extern Bitmap colorChooserSprite;

extern Bitmap neutralFrameSprite;

#define EDITOR_ICON_SPRITE_COUNT 13
extern Bitmap editorIconAtlas;
extern Bitmap editorIconSprite[EDITOR_ICON_SPRITE_COUNT];

extern Bitmap helpScreenImage;

// font
extern Font the_font;

// sons
extern Sound s_click;
extern Sound s_error;
extern Sound s_chat;
extern Sound s_connect;
extern Sound s_player_left;

extern Sound s_bite;
extern Sound s_punch;
extern Sound s_point;
extern Sound s_hatch;
extern Sound s_newAnt;
extern Sound s_get_powerup;
extern Sound s_splash;
extern Sound s_drown;
extern Sound s_powerupDrop;
extern Sound s_create_fire;
extern Sound s_extinguish_fire;
extern Sound s_steal;
extern Sound s_lay_mine;
extern Sound s_explosion;
extern Sound s_small_explosion;
extern Sound s_puff;
extern Sound s_thiefAlert;
extern Sound s_alarm;
extern Sound s_victory;
extern Sound s_defeat;

extern Sound s_reporting1;
extern Sound s_moving1;
extern Sound s_attack1;
extern Sound s_no_no;

// music
extern Sound mus_mainMenu;


extern RenderCache spriteCache;
extern RenderCache spriteCache2;
extern RenderCache textCache;

class GameManager;
extern GameManager * gameManager;

extern int gameState;
extern bool show_debug;
extern bool allow_debug;
extern bool show_map_debug;

// animations
extern int global_explosionAnimationFrames;
extern int global_explosionAnimationDelay;

extern int global_fireAnimationFrames;
extern int global_fireAnimationDelay;

extern int global_powerupDropAnimationFrames;
extern int global_powerupDropAnimationDelay;

#endif
