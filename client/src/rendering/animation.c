#include "animation.h"
#include <string.h>

Animation animation_create(SpriteType sprite_type, int frame_count, int frame_speed) {
    Animation anim = {0};
    anim.sprite_type = sprite_type;
    anim.frame_count = frame_count;
    anim.frame_speed = frame_speed;
    anim.current_frame = 0;
    anim.frame_counter = 0;
    anim.playing = false;
    anim.loop = true;
    return anim;
}

void animation_update(Animation* anim) {
    if (!anim || !anim->playing) return;
    
    anim->frame_counter++;
    
    if (anim->frame_counter >= (60 / anim->frame_speed)) {
        anim->frame_counter = 0;
        anim->current_frame++;
        
        if (anim->current_frame >= anim->frame_count) {
            if (anim->loop) {
                anim->current_frame = 0;
            } else {
                anim->current_frame = anim->frame_count - 1;
                anim->playing = false;
            }
        }
    }
}

void animation_reset(Animation* anim) {
    if (!anim) return;
    anim->current_frame = 0;
    anim->frame_counter = 0;
}

void animation_play(Animation* anim, bool loop) {
    if (!anim) return;
    anim->playing = true;
    anim->loop = loop;
}

void animation_stop(Animation* anim) {
    if (!anim) return;
    anim->playing = false;
}

void animation_draw(Animation* anim, float x, float y, float scale, Color tint) {
    if (!anim) return;
    sprite_manager_draw_frame(anim->sprite_type, anim->current_frame, x, y, scale, tint);
}

AnimatedObject animated_object_create(float x, float y, float scale) {
    AnimatedObject obj = {0};
    obj.x = x;
    obj.y = y;
    obj.scale = scale;
    obj.tint = WHITE;
    obj.animation_count = 0;
    obj.current_animation = 0;
    return obj;
}

int animated_object_add_animation(AnimatedObject* obj, Animation anim) {
    if (!obj || obj->animation_count >= MAX_ANIMATIONS) return -1;
    obj->animations[obj->animation_count] = anim;
    return obj->animation_count++;
}

void animated_object_set_animation(AnimatedObject* obj, int index) {
    if (!obj || index < 0 || index >= obj->animation_count) return;
    
    if (obj->animation_count > 0) {
        animation_stop(&obj->animations[obj->current_animation]);
    }
    
    obj->current_animation = index;
    animation_reset(&obj->animations[obj->current_animation]);
    animation_play(&obj->animations[obj->current_animation], true);
}

void animated_object_update(AnimatedObject* obj) {
    if (!obj || obj->animation_count == 0) return;
    animation_update(&obj->animations[obj->current_animation]);
}

void animated_object_draw(AnimatedObject* obj) {
    if (!obj || obj->animation_count == 0) return;
    animation_draw(&obj->animations[obj->current_animation], 
                   obj->x, obj->y, obj->scale, obj->tint);
}