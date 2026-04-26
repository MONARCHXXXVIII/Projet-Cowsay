#ifndef COWDOOM_GAME_H
#define COWDOOM_GAME_H

#include <stdbool.h>

typedef struct {
    double x;
    double y;
    double dir;
} Player;

typedef struct {
    int width;
    int height;
    const char *tiles;
} Map;

typedef enum {
    GAME_MODE_SURVIVAL = 0,
    GAME_MODE_ATTACK = 1,
} GameMode;

typedef struct {
    int screen_width;
    int screen_height;
    double fov;
    double max_distance;
    double move_speed;
    double turn_speed;
    Player player;
    Map map;
    int hp;
    int ammo;
    int reserve_ammo;
    int kills;
    double survival_time;
    bool in_menu;
    int menu_selection;
    GameMode mode;
    int attack_target_kills;
    int attack_spawned_regular;
    bool attack_boss_spawned;
    bool attack_boss_defeated;
    bool running;
    bool in_game_over;
    bool game_over_victory;
} Game;

void game_init(Game *game, int screen_width, int screen_height);
void game_tick(Game *game, double dt);
void game_render(const Game *game);
void game_shutdown(void);

#endif
