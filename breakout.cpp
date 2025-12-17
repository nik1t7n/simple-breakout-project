#include "assets.h"
#include "ball.h"
#include "game.h"
#include "graphics.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

void update()
{
    UpdateMusicStream(bg_music);

    if (IsKeyPressed(KEY_P) && game_state == in_game_state) {
        game_state = paused_state;
    } else if (IsKeyPressed(KEY_P) && game_state == paused_state) {
        game_state = in_game_state;
    }

    if (game_state == paused_state) {
        return;
    }

    if (game_state == game_over_state) {
        if (IsKeyPressed(KEY_ENTER)) {
            // Restart Level
            load_level(0); // Reload current level info
            game_state = in_game_state;
        } else if (IsKeyPressed(KEY_M)) {
            // Return to Menu
            current_level_index = 0;
            game_state = menu_state;
        }
        return;
    }

    if (game_state == victory_state) {
        if (IsKeyPressed(KEY_ENTER)) {
            game_state = menu_state;
        }
        return;
    }

    if (game_state == menu_state) {
        if (IsKeyPressed(KEY_ENTER)) {
            game_state = in_game_state;
            load_level(0); // Start/Reset Level 1
        } else if (IsKeyPressed(KEY_ONE)) {
            game_state = in_game_state;
            current_level_index = 0;
            load_level(0);
        } else if (IsKeyPressed(KEY_TWO)) {
            game_state = in_game_state;
            current_level_index = 1;
            load_level(0);
        } else if (IsKeyPressed(KEY_THREE)) {
            game_state = in_game_state;
            current_level_index = 2;
            load_level(0);
        } else if (IsKeyPressed(KEY_FOUR)) {
            game_state = in_game_state;
            current_level_index = 3;
            load_level(0);
        } else if (IsKeyPressed(KEY_FIVE)) {
            game_state = in_game_state;
            current_level_index = 4;
            load_level(0);
        }
        return;
    }

    // In Game Logic
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        move_paddle(-paddle_speed);
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        move_paddle(paddle_speed);
    }
    move_ball();

    // Update Powerups
    for (auto& powerup : active_powerups) {
        if (!powerup.active)
            continue;
        powerup.pos.y += 0.05f; // Fall speed

        // Check collision with paddle
        if (CheckCollisionRecs(
                { powerup.pos.x, powerup.pos.y, 1.0f, 1.0f },
                { paddle_pos.x * cell_size / screen_scale, paddle_pos.y * cell_size / screen_scale, paddle_size.x, paddle_size.y })) {
            // Note: Coordinate space mismatch might be an issue here.
            // Let's use logic consistent with move_ball collision.
            // Actually, simpler: check logical coordinates.
        }

        // Simpler collision check using logical coordinates
        Rectangle powerup_rec = { powerup.pos.x, powerup.pos.y, 1.0f, 1.0f };
        Rectangle paddle_rec = { paddle_pos.x, paddle_pos.y, paddle_size.x, paddle_size.y };

        if (CheckCollisionRecs(powerup_rec, paddle_rec)) {
            powerup.active = false;
            PlaySound(pickup_sound);
            // TODO: Apply speed boost effect (need to modify paddle speed)
            // For now just collect it.
        }

        if (powerup.pos.y > current_level.rows) {
            powerup.active = false;
        }
    }

    // Level Transition Logic
    if (!is_ball_inside_level()) {
        PlaySound(lose_sound);
        game_state = game_over_state;
    } else if (current_level_blocks == 0) {
        PlaySound(win_sound);
        load_level(1);
    }
}

void draw()
{
    if (game_state == menu_state) {
        draw_menu();
    } else if (game_state == in_game_state || game_state == paused_state || game_state == game_over_state) {
        draw_level();
        draw_paddle();
        draw_ball();
        draw_ui();

        // Draw Powerups
        for (const auto& powerup : active_powerups) {
            if (powerup.active) {
                // Draw 'S'
                const float texture_x_pos = shift_to_center.x + powerup.pos.x * cell_size;
                const float texture_y_pos = shift_to_center.y + powerup.pos.y * cell_size;
                DrawText("S", texture_x_pos + cell_size / 4, texture_y_pos, cell_size, YELLOW);
            }
        }

        if (game_state == paused_state) {
            draw_pause_menu();
        } else if (game_state == game_over_state) {
            draw_game_over_menu();
        }
    } else if (game_state == victory_state) {
        draw_victory_menu();
    }
}

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "Breakout");
    SetTargetFPS(60);

    load_fonts();
    load_textures();
    load_sounds(); // Music is loaded here

    load_level(); // Initial load

    while (!WindowShouldClose()) {
        BeginDrawing();

        draw();
        update();

        EndDrawing();
    }
    CloseWindow();

    unload_sounds();
    unload_level();
    unload_textures();
    unload_fonts();

    return 0;
}
