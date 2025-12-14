#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"

// Expose metrics
inline Vector2 screen_size;
inline float screen_scale;
inline float cell_size;
inline Vector2 shift_to_center;

void derive_graphics_metrics();

void draw_text(const char* text, float x, float y, float size, Color color);

void draw_menu();
void draw_ui();
void draw_level();
void draw_paddle();
void draw_ball();
void draw_pause_menu();
void draw_victory_menu();
void draw_game_over_menu();

void init_victory_menu();

#endif // GRAPHICS_H
