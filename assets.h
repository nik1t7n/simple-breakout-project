#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

#include "sprite.h"

inline Font menu_font;

inline Texture2D wall_texture;
inline Texture2D void_texture;
inline Texture2D paddle_texture;
inline Texture2D block_texture;

inline sprite ball_sprite;

inline Sound win_sound;
inline Sound lose_sound;
inline Sound pickup_sound;
inline Sound unbreakable_hit_sound;
inline Sound damage_hit_sound;

inline Music bg_music;

void load_fonts();
void unload_fonts();

void load_textures();
void unload_textures();

void load_sounds();
void unload_sounds();

#endif // ASSETS_H
