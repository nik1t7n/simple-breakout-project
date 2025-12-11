#include "level.h"

#include "ball.h"
#include "game.h"
#include "graphics.h"
#include "paddle.h"

#include "raylib.h"

char* current_level_data;

void load_level(const int offset)
{
    current_level_index += offset;

    if (current_level_index >= level_count) {
        game_state = victory_state;
        ClearBackground(BLACK);
        init_victory_menu();
        current_level_index = 0;

        return;
    }

    const size_t rows = levels[current_level_index].rows;
    const size_t columns = levels[current_level_index].columns;
    current_level_blocks = 0;
    active_powerups.clear(); // Clear powerups on level load

    // We need to copy the data because we will modify it (mutable state for durability)
    // and we might need to randomize it.
    delete[] current_level_data; // Delete old data if any
    current_level_data = new char[rows * columns];

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            char cell_info = levels[current_level_index].data[row * columns + column];

            // Handle Random Multi-Hit Block
            if (cell_info == '?') {
                int health = GetRandomValue(2, 11);
                // Convert health to char representation:
                // 1-9 -> '1'-'9'
                // 10 -> 'A'
                // 11 -> 'B'
                if (health < 10) {
                    cell_info = '0' + health;
                } else if (health == 10) {
                    cell_info = 'A';
                } else {
                    cell_info = 'B';
                }
            }

            current_level_data[row * columns + column] = cell_info;

            // Count destroyable blocks
            if (cell_info == BLOCKS || cell_info == SPEED_POWERUP_BLOCK || (cell_info >= '1' && cell_info <= '9') || cell_info == 'A' || cell_info == 'B') {
                ++current_level_blocks;
            }
        }
    }
    current_level = { rows, columns, current_level_data };

    spawn_ball();
    spawn_paddle();

    derive_graphics_metrics();
}

void unload_level()
{
    delete[] current_level_data;
}

bool is_inside_level(const int row, const int column)
{
    return row >= 0 && row < current_level.rows && column >= 0 && column < current_level.columns;
}

char& get_level_cell(const size_t row, const size_t column)
{
    return current_level.data[row * current_level.columns + column];
}

void set_level_cell(const size_t row, const size_t column, const char cell)
{
    get_level_cell(row, column) = cell;
}

bool is_colliding_with_level_cell(const Vector2 pos, const Vector2 size, const char cell)
{
    const Rectangle hitbox = { pos.x, pos.y, size.x, size.y };

    for (int row = static_cast<int>(pos.y); row <= static_cast<int>(pos.y + size.y); ++row) {
        for (int column = static_cast<int>(pos.x); column <= static_cast<int>(pos.x + size.x); ++column) {
            if (!is_inside_level(row, column)) {
                continue;
            }

            if (get_level_cell(row, column) == cell) {
                if (const Rectangle block_hitbox = { static_cast<float>(column), static_cast<float>(row), 1.0f, 1.0f }; CheckCollisionRecs(hitbox, block_hitbox)) {
                    return true;
                }
            }
        }
    }

    return false;
}

char& get_colliding_level_cell(const Vector2 pos, const Vector2 size, const char look_for)
{
    const Rectangle hitbox = { pos.x, pos.y, size.x, size.y };

    for (int row = static_cast<int>(pos.y); row <= static_cast<int>(pos.y + size.y); ++row) {
        for (int column = static_cast<int>(pos.x); column <= static_cast<int>(pos.x + size.x); ++column) {
            if (!is_inside_level(row, column)) {
                continue;
            }

            if (get_level_cell(row, column) == look_for) {
                if (const Rectangle block_hitbox = { static_cast<float>(column), static_cast<float>(row), 1.0f, 1.0f }; CheckCollisionRecs(hitbox, block_hitbox)) {
                    return get_level_cell(row, column);
                }
            }
        }
    }

    return get_level_cell(static_cast<size_t>(pos.x), static_cast<size_t>(pos.y));
}
