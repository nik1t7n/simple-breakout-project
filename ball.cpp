#include "ball.h"
#include "assets.h"
#include "level.h"
#include "paddle.h"

#include "raylib.h"

#include <cmath>
#include <numbers>

void spawn_ball()
{
    for (int column = 0; column < current_level.columns; column++) {
        for (int row = 0; row < current_level.rows; row++) {
            if (get_level_cell(row, column) == BALL) {
                set_level_cell(row, column, VOID);
                ball_pos = { static_cast<float>(column), static_cast<float>(row) };
                constexpr float ball_launch_angle_radians = ball_launch_angle_degrees * (std::numbers::pi_v<float> / 180.0f);
                ball_vel.y = -ball_launch_vel_mag * std::sin(ball_launch_angle_radians);
                ball_vel.x = (rand() % 2 == 0) ? ball_launch_vel_mag * std::cos(ball_launch_angle_radians) : -ball_launch_vel_mag * std::cos(ball_launch_angle_radians);
                goto outer_loop_end;
            }
        }
    }
outer_loop_end:;
}

// Helper to determine collision response
enum CollisionType {
    None,
    Wall,
    Breakable,
    Unbreakable,
    PowerupBlock,
    MultiHit
};

CollisionType get_collision_type(char cell)
{
    if (cell == WALL)
        return Wall;
    if (cell == BLOCKS)
        return Breakable;
    if (cell == UNBREAKABLE_BLOCK)
        return Unbreakable;
    if (cell == SPEED_POWERUP_BLOCK)
        return PowerupBlock;
    if ((cell >= '1' && cell <= '9') || cell == 'A' || cell == 'B')
        return MultiHit;
    return None;
}

void move_ball()
{
    Vector2 next_ball_pos = {
        ball_pos.x + ball_vel.x,
        ball_pos.y + ball_vel.y
    };

    // Check collision with level cells
    bool hit_x = false;
    bool hit_y = false;

    // We need to check all 4 corners or just center? The original code checked just next_pos.
    // Let's stick to the original logic pattern but expanded.

    // Simplification: Check key points or just center? The original `is_colliding_with_level_cell` checks the rect.
    // We need to know WHICH cell was hit to modify it. `get_colliding_level_cell` does this.

    // But `get_colliding_level_cell` takes a 'look_for' char. We have many types now.
    // Let's iterate over the area the ball covers and find what it hits.

    int min_col = static_cast<int>(next_ball_pos.x);
    int max_col = static_cast<int>(next_ball_pos.x + ball_size.x);
    int min_row = static_cast<int>(next_ball_pos.y);
    int max_row = static_cast<int>(next_ball_pos.y + ball_size.y);

    bool collision_handled = false;

    for (int row = min_row; row <= max_row; ++row) {
        for (int column = min_col; column <= max_col; ++column) {
            if (!is_inside_level(row, column))
                continue;

            char& cell = get_level_cell(row, column);
            CollisionType type = get_collision_type(cell);

            if (type == None)
                continue;

            Rectangle block_rect = { static_cast<float>(column), static_cast<float>(row), 1.0f, 1.0f };
            Rectangle ball_rect = { next_ball_pos.x, next_ball_pos.y, ball_size.x, ball_size.y };

            if (CheckCollisionRecs(block_rect, ball_rect)) {

                // Determine bounce direction (simple version based on previous pos)
                // This is a bit tricky with multiple blocks. Let's use the provided simple logic:
                // Check if we were already overlapping in one axis before moving.

                bool overlap_x = (ball_pos.x + ball_size.x > column && ball_pos.x < column + 1.0f);
                bool overlap_y = (ball_pos.y + ball_size.y > row && ball_pos.y < row + 1.0f);

                if (overlap_x && !hit_y) {
                    ball_vel.y = -ball_vel.y;
                    hit_y = true;
                } else if (overlap_y && !hit_x) {
                    ball_vel.x = -ball_vel.x;
                    hit_x = true;
                } else if (!hit_x && !hit_y) {
                    ball_vel.x = -ball_vel.x;
                    ball_vel.y = -ball_vel.y;
                    hit_x = true;
                    hit_y = true;
                }

                // Handle Block Logic
                if (type == Breakable) {
                    cell = VOID;
                    --current_level_blocks;
                    // Optional: hit sound? User didn't specify for standard blocks.
                } else if (type == PowerupBlock) {
                    cell = VOID;
                    --current_level_blocks;
                    active_powerups.push_back({ { static_cast<float>(column), static_cast<float>(row) }, true });
                } else if (type == MultiHit) {
                    PlaySound(damage_hit_sound);
                    if (cell == 'B')
                        cell = 'A';
                    else if (cell == 'A')
                        cell = '9';
                    else if (cell > '1')
                        cell--;
                    else if (cell == '1') {
                        cell = VOID;
                        --current_level_blocks;
                    }
                } else if (type == Unbreakable) {
                    PlaySound(unbreakable_hit_sound);
                }
                // Unbreakable and Wall just bounce (already handled above)

                collision_handled = true;
                break; // Handle one collision per frame to prevent weirdness? Or maybe continue?
                       // Original code handled one type at a time.
            }
        }
        if (collision_handled)
            break;
    }

    // Paddle Collision
    if (!collision_handled && is_colliding_with_paddle(next_ball_pos, ball_size)) {
        ball_vel.y = -std::abs(ball_vel.y);
        // Add some english/x-velocity change based on hit position?
        // simple-breakout doesn't seem to have it, but it makes game better.
        // Keeping it simple as per original requirements unless "better physics" was a goal (it is).
        // Let's add slight deviation.
        float center_paddle = paddle_pos.x + paddle_size.x / 2.0f;
        float center_ball = next_ball_pos.x + ball_size.x / 2.0f;
        ball_vel.x += (center_ball - center_paddle) * 0.05f;
    }

    ball_pos.x += ball_vel.x;
    ball_pos.y += ball_vel.y;

    // Re-verify bounds/stuck correction (simple)
    // Removed specific rounding logic from original for generalized loop,
    // might need to add back if sticking occurs.
}

bool is_ball_inside_level()
{
    return is_inside_level(static_cast<int>(ball_pos.y), static_cast<int>(ball_pos.x));
}
