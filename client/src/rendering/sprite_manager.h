#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    SPRITE_JUNIOR_IDLE,
    SPRITE_JUNIOR_RUN_LEFT,
    SPRITE_JUNIOR_RUN_RIGHT,
    SPRITE_JUNIOR_JUMP_LEFT,
    SPRITE_JUNIOR_JUMP_RIGHT,
    SPRITE_JUNIOR_CLIMB_LEFT,
    SPRITE_JUNIOR_CLIMB_RIGHT,
    SPRITE_JUNIOR_CLIMB_CENTER,
    SPRITE_JUNIOR_DEATH,
    SPRITE_DK_CAGED,
    SPRITE_MARIO_STARE,
    SPRITE_SNAPJAW_RED_UP,
    SPRITE_SNAPJAW_RED_DOWN,
    SPRITE_SNAPJAW_RED_LEFT,
    SPRITE_SNAPJAW_RED_RIGHT,
    SPRITE_SNAPJAW_BLUE_DOWN,
    SPRITE_SNAPJAW_BLUE_LEFT,
    SPRITE_SNAPJAW_BLUE_RIGHT,
    SPRITE_VINE,
    SPRITE_PLATFORM,
    SPRITE_WATER,
    SPRITE_GRASS,
    SPRITE_GRASS_WIDE,
    SPRITE_STEM,
    SPRITE_STEM_WIDE,
    SPRITE_FRUIT_APPLE,
    SPRITE_FRUIT_BANANA,
    SPRITE_FRUIT_MANGO,
    SPRITE_SPLASH,
    SPRITE_POINTS_200,
    SPRITE_POINTS_400,
    SPRITE_POINTS_800,
    SPRITE_LIFE,
    SPRITE_COUNT
} SpriteType;

typedef struct {
    Texture2D texture;
    int frame_width;
    int frame_height;
    int frame_count;
    int spacing;
    bool loaded;
} SpriteSheet;

typedef struct {
    SpriteSheet sprites[SPRITE_COUNT];
    bool initialized;
} SpriteManager;

bool sprite_manager_init(const char* assets_path);
SpriteSheet* sprite_manager_get(SpriteType type);
void sprite_manager_draw_frame(SpriteType type, int frame, float x, float y, float scale, Color tint);
void sprite_manager_cleanup(void);
const char* sprite_manager_get_name(SpriteType type);

#endif