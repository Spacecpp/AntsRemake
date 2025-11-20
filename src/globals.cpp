#include "globals.h"

Shader spriteShader;
Shader pointShader;

//Image gameIcon;

Cursor customCursor;

Bitmap whiteSquareBitmap;
Bitmap outlinedSquareBitmap;

Bitmap antSprite;
Bitmap antSoldierSprite;
Bitmap antFiremanSprite;
Bitmap antRobberSprite;
Bitmap antDiverSprite;
Bitmap antBomberSprite;

Bitmap antFirstFrameSprite[6];

Bitmap antHillDefaultSprite[4];

Bitmap antHillSymetricSprite;
Bitmap antHillFlagSprite;


// terrain
Bitmap waterTexture;
Bitmap dirtTexture;
Bitmap sandTexture;
Bitmap rockyGroundTexture;
Bitmap mudTexture;
Bitmap voidTileTexture;

Bitmap mudTransitionTileSprite;
Bitmap mudToWaterTransitionTileSprite;
Bitmap sandTransitionTileSprite;
Bitmap rockyTransitionTileSprite;

Bitmap fireSprite;
Bitmap landMineSprite;
Bitmap explosionSprite;
Bitmap mudBridgeSprite;

Bitmap terrainAtlas;


Bitmap foodPieceSprite;

Bitmap powerupAtlas;
Bitmap powerupSprite[5];

Bitmap powerupDropSprite;

Bitmap missingSprite;
Bitmap translucentSquaresSprite;
Bitmap selectionCircleSprite;

Bitmap menuBackgroundTexture;

Bitmap HPBarSprite;

Bitmap minimapAntSprite;
Bitmap minimapAntHillSprite;

Bitmap healParticleSprite;

Bitmap deathSmokeSprite;
Bitmap AntTransformationSprite;

Bitmap readyCheckmarkSprite;
Bitmap logoSprite;

// GUI
Bitmap buttonSprite;
Bitmap checkBoxSprite;
Bitmap window9sliceSprite;
Bitmap window9sliceInsideSprite;
Bitmap console9sliceSprite;
Bitmap arrowUpSprite;
Bitmap arrowDownSprite;
Bitmap scrollBoxSprite;
Bitmap sliderBarBitmap;
Bitmap sliderCursorBitmap;
Bitmap colorChooserSprite;

Bitmap neutralFrameSprite;

Bitmap editorIconAtlas;
Bitmap editorIconSprite[EDITOR_ICON_SPRITE_COUNT];

Bitmap helpScreenImage;

// font
Font the_font;

// sons
Sound s_click;
Sound s_error;
Sound s_chat;
Sound s_connect;
Sound s_player_left;

Sound s_bite;
Sound s_punch;
Sound s_point;
Sound s_hatch;
Sound s_newAnt;
Sound s_get_powerup;
Sound s_splash;
Sound s_drown;
Sound s_powerupDrop;
Sound s_create_fire;
Sound s_extinguish_fire;
Sound s_steal;
Sound s_lay_mine;
Sound s_explosion;
Sound s_small_explosion;
Sound s_puff;
Sound s_thiefAlert;
Sound s_alarm;
Sound s_victory;
Sound s_defeat;

Sound s_reporting1;
Sound s_moving1;
Sound s_attack1;
Sound s_no_no;

// music
Sound mus_mainMenu;

RenderCache spriteCache;
RenderCache spriteCache2;
RenderCache textCache;

GameManager * gameManager;

int gameState;
bool show_debug = false;
bool allow_debug = false;
bool show_map_debug = false;

// animations
int global_explosionAnimationFrames = 5;
int global_explosionAnimationDelay = 5;

int global_fireAnimationFrames = 8;
int global_fireAnimationDelay = 5;

int global_powerupDropAnimationFrames = 9;
int global_powerupDropAnimationDelay = 5;
