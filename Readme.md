# Breakout Game - Enhanced Edition

![Breakout](https://i.postimg.cc/FzBNx6FK/Screenshot-20251103-175907.png)

---

This is an enhanced version of the classic Breakout game, implemented in C++ using the raylib library. It features multiple levels, various block types, powerups, and sound effects to provide an engaging arcade experience.

## Features Implemented

### Core Requirements
*   **Menu Screen**: A starting screen with the game title and options to Start.
*   **Game Screen**: The main gameplay area featuring the paddle, ball, and blocks.
*   **Pause Screen**: Press `ESC` to pause the game.
*   **Victory Screen**: A congratulatory screen upon completing all levels.

### Additional Features ("3+3+3+1" Rule)

#### 1. Additional Levels (3+)
I have implemented **5 Levels** in total, introducing new mechanics progressively:
*   **Level 3**: Introduces Unbreakable Blocks ('X') and Speed Powerups ('S').
*   **Level 4**: Introduces Random Multi-Hit Blocks ('?').
*   **Level 5**: "Chaos" modes combining all elements.

#### 2. New Game Elements (3)
*   **Random Multi-Hit Blocks ('?')**: These blocks spawn with a random health value between 2 and 11. The number displayed on the block represents hits remaining. They change color/number as they take damage.
*   **Unbreakable Blocks ('X')**: Solid blocks that the ball bounces off but cannot destroy. They act as permanent obstacles.
*   **Speed Powerup ('S')**: Breaking these blocks drops a Speed Powerup. (Note: Currently implemented as score/collection mechanic, visual 'S' drop).

#### 3. Game Over Screen
*   A dedicated **Game Over** screen appears when the ball falls below the paddle.
*   Options to **Try Again** (restart current level) or return to **Menu**.

#### 4. Audio
*   **Background Music**: "NightShade" by AdhesiveWombat (Royalty Free) plays during gameplay.
*   **Sound Effects**: Win and Lose sounds are included.

## Controls
*   **A / Left Arrow**: Move Paddle Left
*   **D / Right Arrow**: Move Paddle Right
*   **ESC**: Pause / Resume
*   **ENTER**: Select / Restart / Try Again
*   **M**: Return to Menu (from Game Over)

## Compilation & Running
The project is set up with CMake. Ensure that the working directory is set to the project root so the game can access `data/` assets.

```bash
mkdir build
cd build
cmake ..
make
./breakout
```

## Credits
*   Game logic and C++ implementation by [Your Name]
*   Music: "NightShade" by AdhesiveWombat
*   Base Project Structure: [Course Instructors]
