# Архитектура проекта Breakout

## Содержание
1. [Обзор проекта](#обзор-проекта)
2. [Структура файлов](#структура-файлов)
3. [Основные компоненты](#основные-компоненты)
4. [Игровой цикл](#игровой-цикл)
5. [Система состояний](#система-состояний)
6. [Система уровней](#система-уровней)
7. [Физика и коллизии](#физика-и-коллизии)
8. [Графическая подсистема](#графическая-подсистема)
9. [Аудио система](#аудио-система)
10. [Новые игровые элементы](#новые-игровые-элементы)

---

## Обзор проекта

Breakout — классическая аркадная игра, реализованная на C++ с использованием библиотеки **raylib**. 

### Используемые технологии:
- **Язык**: C++17
- **Графика/Ввод/Аудио**: raylib 4.x
- **Сборка**: CMake
- **Менеджер пакетов**: vcpkg (для raylib)

### Принципы разработки:
- Процедурный стиль программирования (не ООП)
- Модульная структура (разделение по файлам)
- Data-Driven подход для уровней

---

## Структура файлов

```
simple-breakout-project/
├── breakout.cpp        # Точка входа, главный цикл
├── game.h              # Константы, типы данных, данные уровней
├── level.cpp / level.h # Загрузка и управление уровнями
├── ball.cpp / ball.h   # Логика мяча и коллизий
├── paddle.cpp / paddle.h # Управление ракеткой
├── graphics.cpp / graphics.h # Отрисовка всех элементов
├── assets.cpp / assets.h # Управление ресурсами
│
├── data/
│   ├── fonts/          # Шрифты (.ttf)
│   ├── images/         # Текстуры (.png)
│   ├── music/          # Фоновая музыка (.mp3)
│   └── sounds/         # Звуковые эффекты (.wav)
│
├── CMakeLists.txt      # Конфигурация сборки
├── Readme.md           # Описание проекта
└── architecture.md     # Этот файл
```

### Описание каждого файла:

| Файл | Назначение |
|------|------------|
| `breakout.cpp` | Содержит `main()`, `update()`, `draw()` — основной игровой цикл |
| `game.h` | Определяет константы блоков, структуры данных, массивы уровней |
| `level.cpp/h` | Функции загрузки уровней, проверки границ, доступа к клеткам |
| `ball.cpp/h` | Движение мяча, обнаружение и обработка столкновений |
| `paddle.cpp/h` | Спавн ракетки, обработка ввода для движения |
| `graphics.cpp/h` | Все функции отрисовки: меню, уровень, UI, экраны |
| `assets.cpp/h` | Загрузка/выгрузка текстур, шрифтов, звуков, музыки |

---

## Основные компоненты

### 1. Типы блоков (`game.h`)

```cpp
constexpr char VOID = ' ';    // Пустая клетка (проходимая)
constexpr char WALL = '#';    // Стена (непроходимая, неразрушимая)
constexpr char BLOCKS = '@';  // Обычный разрушаемый блок
constexpr char PADDLE = 'P';  // Позиция спавна ракетки
constexpr char BALL = '*';    // Позиция спавна мяча
constexpr char BOUNDARY = '!';// Нижняя граница (проигрыш при касании)

// Новые типы блоков:
constexpr char RANDOM_MULTI_HIT_BLOCK = '?';  // Многоразовый блок
constexpr char UNBREAKABLE_BLOCK = 'X';       // Неразрушимый блок  
constexpr char SPEED_POWERUP_BLOCK = 'S';     // Блок с бонусом
```

**Объяснение**: Каждая клетка уровня представлена одним символом. Это позволяет легко редактировать уровни прямо в коде и визуально понимать их структуру.

### 2. Структура уровня (`game.h`)

```cpp
struct level {
    size_t rows = 0;      // Количество строк
    size_t columns = 0;   // Количество столбцов
    char* data = nullptr; // Указатель на массив символов
};
```

**Объяснение**: Уровень — это одномерный массив символов, представляющий двумерную сетку. Доступ к клетке `[row][col]` осуществляется по формуле: `data[row * columns + col]`.

### 3. Структура бонуса (`game.h`)

```cpp
struct Powerup {
    Vector2 pos;   // Позиция в игровых координатах
    bool active;   // Активен ли бонус (падает/существует)
};

inline std::vector<Powerup> active_powerups;  // Список активных бонусов
```

---

## Игровой цикл

Расположен в `breakout.cpp`. Следует классическому паттерну Game Loop:

```
┌─────────────────────────────────────────┐
│                 main()                  │
├─────────────────────────────────────────┤
│  1. InitWindow() — создание окна        │
│  2. load_*() — загрузка ресурсов        │
│  3. while (!WindowShouldClose()) {      │
│       BeginDrawing();                   │
│       draw();   ← Отрисовка             │
│       update(); ← Логика                │
│       EndDrawing();                     │
│     }                                   │
│  4. unload_*() — выгрузка ресурсов      │
│  5. CloseWindow()                       │
└─────────────────────────────────────────┘
```

### Функция `update()`

```cpp
void update() {
    // 1. Обновление музыки (для стриминга)
    UpdateMusicStream(bg_music);
    
    // 2. Обработка паузы (KEY_P)
    if (IsKeyPressed(KEY_P) && game_state == in_game_state)
        game_state = paused_state;
    
    // 3. Логика в зависимости от состояния
    switch (game_state) {
        case menu_state:     // Обработка выбора уровня
        case in_game_state:  // Движение, коллизии, проверка победы
        case paused_state:   // Ничего не делаем
        case game_over_state:// Ожидание нажатия (рестарт/меню)
        case victory_state:  // Ожидание возврата в меню
    }
}
```

### Функция `draw()`

```cpp
void draw() {
    // Отрисовка зависит от текущего состояния
    if (game_state == menu_state) {
        draw_menu();
    } 
    else if (game_state == in_game_state || ...) {
        draw_level();   // Фон + блоки
        draw_paddle();  // Ракетка
        draw_ball();    // Мяч
        draw_ui();      // Интерфейс (счёт, уровень)
        
        // Оверлеи для паузы/game over
        if (game_state == paused_state) draw_pause_menu();
        if (game_state == game_over_state) draw_game_over_menu();
    }
    else if (game_state == victory_state) {
        draw_victory_menu();
    }
}
```

---

## Система состояний

Игра использует паттерн **State Machine** (конечный автомат):

```
                    ┌──────────────┐
        ┌───────────│  menu_state  │◄───────────┐
        │           └──────┬───────┘            │
        │                  │ ENTER / 1-5        │
        │                  ▼                    │
        │           ┌─────────────────┐         │
        │  ┌───────►│  in_game_state  │◄───┐    │
        │  │        └────┬───────┬────┘    │    │
        │  │             │       │         │    │
        │  │ KEY_P       │       │    KEY_P│    │
        │  │             │       │         │    │
        │  │        ┌────▼───┐   │    ┌────┴────┐
        │  └────────│ paused │   │    │         │
        │           └────────┘   │    │         │
        │                        │    │         │
        │    Ball out of bounds  │    │ All blocks│
        │                        ▼    │ destroyed │
        │           ┌───────────────┐ │         │
        │    KEY_M  │game_over_state│ │         │
        └───────────┤               │ │         │
                    └───────┬───────┘ │         │
                            │ ENTER   │         │
                            ▼         ▼         │
                    ┌─────────────────────┐     │
                    │    victory_state    │─────┘
                    └─────────────────────┘
                            │ ENTER
                            ▼
                      (return to menu)
```

### Определение состояний (`game.h`)

```cpp
enum game_state {
    menu_state,       // Главное меню
    in_game_state,    // Игровой процесс
    paused_state,     // Пауза
    victory_state,    // Победа (все уровни пройдены)
    game_over_state   // Проигрыш (мяч упал)
};

inline game_state game_state = menu_state;  // Начальное состояние
```

---

## Система уровней

### Хранение уровней (`game.h`)

Уровни определены как массивы символов:

```cpp
inline char level_1_data[] = {
    '#', '#', '#', '#', '#', '#', '#', '#', '#',
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', '@', '@', '@', '@', '@', ' ', '#',  // @ = блоки
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#',
    '#', ' ', ' ', ' ', '*', ' ', ' ', ' ', '#',  // * = мяч
    '#', ' ', ' ', 'P', ' ', ' ', ' ', ' ', '#',  // P = ракетка
    '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#'
};

inline level level_1 = { 7, 9, level_1_data };  // rows, cols, data
```

### Загрузка уровня (`level.cpp`)

```cpp
void load_level(int offset = 0) {
    current_level_index += offset;
    
    // Проверка на победу
    if (current_level_index >= level_count) {
        game_state = victory_state;
        return;
    }
    
    // Копирование данных уровня (чтобы можно было изменять)
    size_t size = levels[current_level_index].rows * 
                  levels[current_level_index].columns;
    current_level.data = new char[size];
    memcpy(current_level.data, levels[current_level_index].data, size);
    
    // Рандомизация '?' блоков
    for (каждая клетка) {
        if (cell == '?') {
            int health = rand() % 10 + 2;  // 2-11
            // Кодируем: 2-9 → '2'-'9', 10 → 'A', 11 → 'B'
            cell = (health > 9) ? ('A' + health - 10) : ('0' + health);
        }
    }
    
    // Подсчёт разрушаемых блоков
    current_level_blocks = count(BLOCKS, SPEED_POWERUP_BLOCK, multi-hit...);
    
    // Спавн мяча и ракетки
    spawn_ball();
    spawn_paddle();
}
```

**Важно**: Мы создаём копию данных уровня, потому что при разрушении блоков мы меняем символы на `VOID`. Без копии изменения затронули бы оригинальные данные.

---

## Физика и коллизии

### Движение мяча (`ball.cpp`)

```cpp
void move_ball() {
    // 1. Обновление позиции
    ball_pos.x += ball_vel.x * GetFrameTime() * 60;
    ball_pos.y += ball_vel.y * GetFrameTime() * 60;
    
    // 2. Проверка коллизий для всех углов мяча
    // (мяч имеет размер, не точка)
    
    // 3. При столкновении:
    //    - Изменить направление (отражение)
    //    - Обработать тип блока
}
```

### Типы коллизий

```cpp
enum CollisionType {
    None,           // Нет столкновения
    Wall,           // Стена (просто отскок)
    Breakable,      // Обычный блок (уничтожить)
    PowerupBlock,   // Блок с бонусом (уничтожить + создать бонус)
    MultiHit,       // Многоразовый (уменьшить здоровье)
    Unbreakable     // Неразрушимый (только отскок)
};
```

### Обработка коллизий

```cpp
// Определение типа по символу
CollisionType get_collision_type(char cell) {
    if (cell == WALL || cell == '#') return Wall;
    if (cell == BLOCKS) return Breakable;
    if (cell == SPEED_POWERUP_BLOCK) return PowerupBlock;
    if (cell == UNBREAKABLE_BLOCK) return Unbreakable;
    if (cell >= '1' && cell <= '9' || cell == 'A' || cell == 'B') 
        return MultiHit;
    return None;
}

// Обработка
if (type == Breakable) {
    cell = VOID;
    --current_level_blocks;
}
else if (type == MultiHit) {
    PlaySound(damage_hit_sound);
    if (cell == 'B') cell = 'A';
    else if (cell == 'A') cell = '9';
    else if (cell > '1') cell--;
    else {
        cell = VOID;
        --current_level_blocks;
    }
}
else if (type == Unbreakable) {
    PlaySound(unbreakable_hit_sound);
    // Только отскок, блок не меняется
}
```

### Отражение мяча

```cpp
// Определяем сторону столкновения
if (hit_from_side) ball_vel.x *= -1;  // Горизонтальное отражение
if (hit_from_top_bottom) ball_vel.y *= -1;  // Вертикальное отражение
```

---

## Графическая подсистема

### Масштабирование (`graphics.cpp`)

```cpp
void derive_graphics_metrics_from_loaded_level() {
    // Рассчитываем размер клетки, чтобы уровень 
    // поместился на экране с сохранением пропорций
    
    float scale_x = GetScreenWidth() / current_level.columns;
    float scale_y = GetScreenHeight() / current_level.rows;
    cell_size = min(scale_x, scale_y);
    
    // Центрирование уровня
    float level_width = current_level.columns * cell_size;
    float level_height = current_level.rows * cell_size;
    shift_to_center.x = (GetScreenWidth() - level_width) / 2;
    shift_to_center.y = (GetScreenHeight() - level_height) / 2;
}
```

### Отрисовка уровня

```cpp
void draw_level() {
    ClearBackground(BLACK);
    
    for (size_t row = 0; row < current_level.rows; ++row) {
        for (size_t col = 0; col < current_level.columns; ++col) {
            char cell = get_level_cell(row, col);
            
            float x = shift_to_center.x + col * cell_size;
            float y = shift_to_center.y + row * cell_size;
            
            switch (cell) {
                case WALL:
                    DrawTexture(wall_texture, x, y, WHITE);
                    break;
                case BLOCKS:
                    DrawTexture(block_texture, x, y, WHITE);
                    break;
                case UNBREAKABLE_BLOCK:
                    DrawTexture(wall_texture, x, y, DARKGRAY);
                    break;
                case SPEED_POWERUP_BLOCK:
                    DrawTexture(block_texture, x, y, ORANGE);
                    break;
                // Многоразовые блоки: текстура + число здоровья
                case '1'...'9':
                case 'A': case 'B':
                    DrawTexture(block_texture, x, y, PURPLE);
                    DrawText(cell, x + cell_size/3, y, cell_size, BLACK);
                    break;
            }
        }
    }
}
```

---

## Аудио система

### Загрузка (`assets.cpp`)

```cpp
void load_sounds() {
    InitAudioDevice();  // Инициализация аудио подсистемы raylib
    
    // Звуковые эффекты (загружаются полностью в память)
    win_sound = LoadSound("data/sounds/win.wav");
    lose_sound = LoadSound("data/sounds/lose.wav");
    pickup_sound = LoadSound("data/sounds/pickupCoin.wav");
    unbreakable_hit_sound = LoadSound("data/sounds/blockhit.wav");
    damage_hit_sound = LoadSound("data/sounds/hitHurt.wav");
    
    // Музыка (стримится с диска)
    bg_music = LoadMusicStream("data/music/AdhesiveWombat-NightShade.mp3");
    PlayMusicStream(bg_music);
    SetMusicVolume(bg_music, 0.5f);
}
```

### Использование

```cpp
// В update():
UpdateMusicStream(bg_music);  // Обязательно каждый кадр!

// При событиях:
PlaySound(win_sound);           // Победа
PlaySound(lose_sound);          // Проигрыш  
PlaySound(pickup_sound);        // Подбор бонуса
PlaySound(unbreakable_hit_sound); // Удар о неразрушимый
PlaySound(damage_hit_sound);    // Повреждение многоразового блока
```

---

## Новые игровые элементы

### 1. Многоразовые блоки ('?')

**Механика**:
- При загрузке уровня '?' заменяется на случайное число (2-11)
- Число отображается на блоке
- Каждое попадание уменьшает число на 1
- При достижении 0 блок уничтожается

**Кодирование здоровья**:
```
Здоровье 2  → символ '2'
Здоровье 9  → символ '9'
Здоровье 10 → символ 'A'
Здоровье 11 → символ 'B'
```

### 2. Неразрушимые блоки ('X')

**Механика**:
- Мяч отскакивает
- Блок никогда не исчезает
- Не учитывается в `current_level_blocks`
- Отображается тёмно-серым цветом

### 3. Бонусы скорости ('S')

**Механика**:
1. При разрушении блока создаётся объект `Powerup`
2. Бонус падает вниз (`pos.y += 0.05f`)
3. При касании ракетки: `PlaySound(pickup_sound)`
4. Бонус деактивируется

```cpp
// Создание бонуса при разрушении
if (type == PowerupBlock) {
    cell = VOID;
    --current_level_blocks;
    active_powerups.push_back({ 
        { (float)column, (float)row }, 
        true 
    });
}

// Обновление бонусов
for (auto& powerup : active_powerups) {
    if (!powerup.active) continue;
    powerup.pos.y += 0.05f;  // Падение
    
    // Проверка подбора
    if (CheckCollisionRecs(powerup_rect, paddle_rect)) {
        powerup.active = false;
        PlaySound(pickup_sound);
    }
}
```

---

## Управление

| Клавиша | Действие | Где работает |
|---------|----------|--------------|
| A / ← | Движение влево | in_game_state |
| D / → | Движение вправо | in_game_state |
| P | Пауза / Продолжить | in_game_state, paused_state |
| ENTER | Старт / Рестарт | menu_state, game_over_state, victory_state |
| M | Вернуться в меню | game_over_state |
| 1-5 | Выбор уровня | menu_state |
| ESC | Выход из игры | Везде (raylib default) |

---

## Заключение

Проект демонстрирует:
- **Модульную архитектуру** — каждый файл отвечает за свою область
- **State Machine** — чёткое разделение логики по состояниям
- **Data-Driven подход** — уровни как данные, не код
- **Правильное управление ресурсами** — load/unload паттерн
- **Расширяемость** — легко добавить новые типы блоков/бонусов

Код следует принципам структурного программирования с использованием глобальных переменных (объявленных как `inline` для соблюдения ODR) и свободных функций.
