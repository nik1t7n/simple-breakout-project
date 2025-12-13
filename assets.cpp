#include "assets.h"

#include "raylib.h"

void load_fonts()
{
    menu_font = LoadFontEx("data/fonts/ARCADECLASSIC.TTF", 256, nullptr, 0);
}

void unload_fonts()
{
    UnloadFont(menu_font);
}

void load_textures()
{
    wall_texture = LoadTexture("data/images/wall.png");
    void_texture = LoadTexture("data/images/void.png");
    block_texture = LoadTexture("data/images/block.png");
    paddle_texture = LoadTexture("data/images/paddle.png");
    ball_sprite = load_sprite("data/images/ball/ball", ".png", 8, true, 10);
}

void unload_textures()
{
    UnloadTexture(wall_texture);
    UnloadTexture(void_texture);
    UnloadTexture(block_texture);
    UnloadTexture(paddle_texture);
    unload_sprite(ball_sprite);
}

void load_sounds()
{
    InitAudioDevice();
    win_sound = LoadSound("data/sounds/win.wav");
    lose_sound = LoadSound("data/sounds/lose.wav");
    pickup_sound = LoadSound("data/sounds/pickupCoin.wav");
    unbreakable_hit_sound = LoadSound("data/sounds/blockhit.wav");
    damage_hit_sound = LoadSound("data/sounds/hitHurt.wav");

    bg_music = LoadMusicStream("data/music/AdhesiveWombat-NightShade-NO-COPYRIGHT-8-bit-Music.mp3");
    PlayMusicStream(bg_music);
    SetMusicVolume(bg_music, 0.5f);
}

void unload_sounds()
{
    UnloadSound(win_sound);
    UnloadSound(lose_sound);
    UnloadSound(pickup_sound);
    UnloadSound(unbreakable_hit_sound);
    UnloadSound(damage_hit_sound);
    UnloadMusicStream(bg_music);
    CloseAudioDevice();
}
