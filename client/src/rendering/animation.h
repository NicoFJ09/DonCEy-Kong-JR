#ifndef ANIMATION_H
#define ANIMATION_H

#include "sprite_manager.h"
#include <stdbool.h>

#define MAX_ANIMATIONS 32

typedef struct {
    SpriteType sprite_type;
    int current_frame;
    int frame_count;
    int frame_counter;
    int frame_speed;
    bool playing;
    bool loop;
} Animation;

typedef struct {
    Animation animations[MAX_ANIMATIONS];
    int current_animation;
    int animation_count;
    float x, y;
    float scale;
    Color tint;
} AnimatedObject;

Animation animation_create(SpriteType sprite_type, int frame_count, int frame_speed);
void animation_update(Animation* anim);
void animation_reset(Animation* anim);
void animation_play(Animation* anim, bool loop);
void animation_stop(Animation* anim);
void animation_draw(Animation* anim, float x, float y, float scale, Color tint);

AnimatedObject animated_object_create(float x, float y, float scale);
int animated_object_add_animation(AnimatedObject* obj, Animation anim);
void animated_object_set_animation(AnimatedObject* obj, int index);
void animated_object_update(AnimatedObject* obj);
void animated_object_draw(AnimatedObject* obj);

#endif