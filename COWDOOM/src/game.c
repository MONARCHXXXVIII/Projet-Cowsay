#include "game.h"
#include "input.h"
#include "video.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PI 3.14159265358979323846

typedef struct {
    double x;
    double y;
    int hp;
    bool boss;
    bool alive;
    double hit_flash_timer;
} Enemy;

#define ENEMY_CAPACITY 64
#define ENEMY_INITIAL_COUNT 4
#define ENEMY_MAX_HP 3
#define BOSS_HP 18
#define MAG_CAPACITY 12

#define SPAWN_INTERVAL_MIN 0.7
#define SPAWN_ACCELERATION 0.96

static Enemy s_enemies[ENEMY_CAPACITY];

static const double s_initial_spawns[ENEMY_INITIAL_COUNT][2] = {
    {8.5, 6.5},
    {14.5, 4.5},
    {11.5, 10.5},
    {5.5, 9.5},
};

static double s_contact_damage_cooldown = 0.0;
static double s_spawn_timer = 0.0;
static double s_spawn_interval = 4.0;
static double s_shoot_flash_timer = 0.0;
static double s_damage_flash_timer = 0.0;
static int    s_last_final_kills = 0;
static double s_last_final_time  = 0.0;

/* 16x12: mode Survie (ancien mode) */
static const char s_world_survival[] =
    "################"
    "#..............#"
    "#...##.........#"
    "#..............#"
    "#.......###....#"
    "#..............#"
    "#....###.......#"
    "#..............#"
    "#......#.......#"
    "#......#.......#"
    "#..............#"
    "################";

/* 24x16: mode Attaque (map plus grande) */
static const char s_world_attack[] =
    "########################"
    "#......................#"
    "#..####.......####.....#"
    "#..#..........#........#"
    "#..#..######..#..####..#"
    "#.....#....#.....#.....#"
    "#......#........#......#"
    "#..#######..##..####...#"
    "#...........##.........#"
    "#..#####........#####..#"
    "#..#..##########..#....#"
    "#..#..............#....#"
    "#..######..##..######..#"
    "#......................#"
    "#....####..............#"
    "########################";

static bool map_is_wall(const Game *game, double x, double y) {
    int mx = (int)x;
    int my = (int)y;

    if (mx < 0 || my < 0 || mx >= game->map.width || my >= game->map.height) {
        return true;
    }

    return game->map.tiles[my * game->map.width + mx] == '#';
}

static void put_char(char *buffer, int width, int height, int x, int y, char c) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }
    buffer[y * width + x] = c;
}

static char shade_for_distance(double d, double max_d) {
    double t = d / max_d;
    if (t < 0.05) return '@';
    if (t < 0.11) return '#';
    if (t < 0.18) return 'H';
    if (t < 0.28) return 'X';
    if (t < 0.40) return '=';
    if (t < 0.54) return '-';
    if (t < 0.70) return ':';
    if (t < 0.86) return ',';
    return '.';
}

static char darken_wall_shade(char c) {
    switch (c) {
        case '@': return '#';
        case '#': return 'H';
        case 'H': return 'X';
        case 'X': return '=';
        case '=': return '-';
        case '-': return ':';
        case ':': return ',';
        case ',': return '.';
        default: return c;
    }
}

static void clear_enemies(void) {
    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        s_enemies[i].alive = false;
        s_enemies[i].boss = false;
        s_enemies[i].hp = 0;
        s_enemies[i].x = 0.0;
        s_enemies[i].y = 0.0;
        s_enemies[i].hit_flash_timer = 0.0;
    }
}

static int count_alive_enemies(void) {
    int alive = 0;
    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (s_enemies[i].alive) {
            alive++;
        }
    }
    return alive;
}

static int count_alive_regular_enemies(void) {
    int alive = 0;
    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (s_enemies[i].alive && !s_enemies[i].boss) {
            alive++;
        }
    }
    return alive;
}

static bool has_line_of_sight(const Game *game, double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double dist = sqrt(dx * dx + dy * dy);
    if (dist < 0.001) {
        return true;
    }

    for (double t = 0.25; t < dist - 0.2; t += 0.1) {
        double tx = x1 + (dx / dist) * t;
        double ty = y1 + (dy / dist) * t;
        if (map_is_wall(game, tx, ty)) {
            return false;
        }
    }
    return true;
}

static int find_free_enemy_slot(void) {
    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (!s_enemies[i].alive) {
            return i;
        }
    }
    return -1;
}

static void spawn_enemy_at(int idx, double x, double y, int hp, bool boss) {
    s_enemies[idx].x = x;
    s_enemies[idx].y = y;
    s_enemies[idx].hp = hp;
    s_enemies[idx].boss = boss;
    s_enemies[idx].alive = true;
}

static bool try_spawn_enemy_random(const Game *game, int hp, bool boss, double min_player_dist) {
    int idx = find_free_enemy_slot();
    if (idx < 0) {
        return false;
    }

    for (int attempt = 0; attempt < 300; ++attempt) {
        int mx = 1 + rand() % (game->map.width - 2);
        int my = 1 + rand() % (game->map.height - 2);
        double ex = (double)mx + 0.5;
        double ey = (double)my + 0.5;

        if (map_is_wall(game, ex, ey)) {
            continue;
        }

        double pdx = ex - game->player.x;
        double pdy = ey - game->player.y;
        double pd = sqrt(pdx * pdx + pdy * pdy);
        if (pd < min_player_dist) {
            continue;
        }

        bool too_close = false;
        for (int i = 0; i < ENEMY_CAPACITY; ++i) {
            if (!s_enemies[i].alive) {
                continue;
            }
            double dx = ex - s_enemies[i].x;
            double dy = ey - s_enemies[i].y;
            if (dx * dx + dy * dy < 0.64) {
                too_close = true;
                break;
            }
        }
        if (too_close) {
            continue;
        }

        spawn_enemy_at(idx, ex, ey, hp, boss);
        return true;
    }

    return false;
}

static void start_mode(Game *game, GameMode mode) {
    game->mode = mode;
    game->in_menu = false;

    game->fov = PI / 3.0;
    game->max_distance = 20.0;
    game->move_speed = 3.5;
    game->turn_speed = 2.4;

    game->player.x = 2.5;
    game->player.y = 2.5;
    game->player.dir = 0.0;

    game->hp = 100;
    game->ammo = MAG_CAPACITY;
    game->reserve_ammo = (mode == GAME_MODE_ATTACK) ? 84 : 72;
    game->kills = 0;
    game->survival_time = 0.0;

    game->attack_target_kills = 24;
    game->attack_spawned_regular = 0;
    game->attack_boss_spawned = false;
    game->attack_boss_defeated = false;

    clear_enemies();

    if (mode == GAME_MODE_SURVIVAL) {
        game->map.width = 16;
        game->map.height = 12;
        game->map.tiles = s_world_survival;

        for (int i = 0; i < ENEMY_INITIAL_COUNT; ++i) {
            int idx = find_free_enemy_slot();
            if (idx >= 0) {
                spawn_enemy_at(idx, s_initial_spawns[i][0], s_initial_spawns[i][1], ENEMY_MAX_HP, false);
            }
        }

        s_spawn_interval = 4.0;
    } else {
        game->map.width = 24;
        game->map.height = 16;
        game->map.tiles = s_world_attack;

        for (int i = 0; i < 6; ++i) {
            if (try_spawn_enemy_random(game, ENEMY_MAX_HP, false, 3.0)) {
                game->attack_spawned_regular++;
            }
        }

        s_spawn_interval = 1.9;
    }

    s_spawn_timer = 0.0;
    s_contact_damage_cooldown = 0.0;
}

static void draw_enemies(const Game *game, char *screen, const double *depth_buffer) {
    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (!s_enemies[i].alive) {
            continue;
        }

        double dx = s_enemies[i].x - game->player.x;
        double dy = s_enemies[i].y - game->player.y;
        double dist = sqrt(dx * dx + dy * dy);
        if (dist < 0.2 || dist > game->max_distance) {
            continue;
        }

        double angle_to_enemy = atan2(dy, dx);
        double rel = angle_to_enemy - game->player.dir;
        while (rel > PI) rel -= 2.0 * PI;
        while (rel < -PI) rel += 2.0 * PI;

        if (fabs(rel) > game->fov * 0.5) {
            continue;
        }

        int sx = (int)((rel / (game->fov * 0.5)) * (game->screen_width / 2.0) + (game->screen_width / 2.0));
        if (sx < 0 || sx >= game->screen_width) {
            continue;
        }

        if (depth_buffer[sx] < dist) {
            continue;
        }

        int sprite_h = (int)(game->screen_height / dist);
        if (s_enemies[i].boss) {
            sprite_h = (int)(sprite_h * 1.35);
        }
        if (sprite_h < 3) {
            sprite_h = 3;
        }

        int top = game->screen_height / 2 - sprite_h / 2;
        int bottom = game->screen_height / 2 + sprite_h / 2;

        int span = (bottom > top) ? (bottom - top) : 1;
        int sw = game->screen_width;
        int sh = game->screen_height;
        bool flash = (s_enemies[i].hit_flash_timer > 0.0);

        for (int y = top; y <= bottom; ++y) {
            double frac = (double)(y - top) / (double)span;

            if (s_enemies[i].boss) {
                /* Boss: section 0-20% couronne large, 20-72% corps large, 72-100% pattes */
                char body = flash ? '!' : '@';
                char side = flash ? '!' : 'O';
                char wing = flash ? '!' : 'W';
                if (frac < 0.20) {
                    /* Couronne / tete large */
                    put_char(screen, sw, sh, sx - 3, y, 'B');
                    put_char(screen, sw, sh, sx - 2, y, wing);
                    put_char(screen, sw, sh, sx - 1, y, body);
                    put_char(screen, sw, sh, sx,     y, body);
                    put_char(screen, sw, sh, sx + 1, y, body);
                    put_char(screen, sw, sh, sx + 2, y, wing);
                    put_char(screen, sw, sh, sx + 3, y, 'B');
                } else if (frac < 0.72) {
                    /* Corps principal */
                    put_char(screen, sw, sh, sx - 3, y, 'B');
                    put_char(screen, sw, sh, sx - 2, y, wing);
                    put_char(screen, sw, sh, sx - 1, y, side);
                    put_char(screen, sw, sh, sx,     y, body);
                    put_char(screen, sw, sh, sx + 1, y, side);
                    put_char(screen, sw, sh, sx + 2, y, wing);
                    put_char(screen, sw, sh, sx + 3, y, 'B');
                } else {
                    /* Pattes ecartees */
                    put_char(screen, sw, sh, sx - 3, y, 'B');
                    put_char(screen, sw, sh, sx - 2, y, wing);
                    put_char(screen, sw, sh, sx + 2, y, wing);
                    put_char(screen, sw, sh, sx + 3, y, 'B');
                }
            } else {
                /* Vache normale:
                   0-22%  : tete etroite (3 cols)
                   22-65% : corps large  (5 cols)
                   65-100%: pattes (2 cols + bords) */
                char body  = flash ? '!' : 'M';
                char sideL = flash ? '!' : 'u';
                char sideR = flash ? '!' : 'h';
                char face  = flash ? '!' : 'O';
                if (frac < 0.22) {
                    /* Tete */
                    put_char(screen, sw, sh, sx - 1, y, 'B');
                    put_char(screen, sw, sh, sx,     y, face);
                    put_char(screen, sw, sh, sx + 1, y, 'B');
                } else if (frac < 0.65) {
                    /* Corps */
                    put_char(screen, sw, sh, sx - 2, y, 'B');
                    put_char(screen, sw, sh, sx - 1, y, sideL);
                    put_char(screen, sw, sh, sx,     y, body);
                    put_char(screen, sw, sh, sx + 1, y, sideR);
                    put_char(screen, sw, sh, sx + 2, y, 'B');
                } else {
                    /* Pattes */
                    put_char(screen, sw, sh, sx - 2, y, 'B');
                    put_char(screen, sw, sh, sx - 1, y, sideL);
                    put_char(screen, sw, sh, sx + 1, y, sideR);
                    put_char(screen, sw, sh, sx + 2, y, 'B');
                }
            }
        }

        /* Ligne de bord haut sur la largeur du corps */
        {
            int bx_lo = s_enemies[i].boss ? sx - 3 : sx - 2;
            int bx_hi = s_enemies[i].boss ? sx + 3 : sx + 2;
            for (int bx = bx_lo; bx <= bx_hi; ++bx) {
                put_char(screen, sw, sh, bx, top, 'B');
            }
        }
    }
}

void game_init(Game *game, int screen_width, int screen_height) {
    game->screen_width = screen_width;
    game->screen_height = screen_height;
    game->running = true;
    game->in_menu = true;
    game->in_game_over = false;
    game->game_over_victory = false;
    game->menu_selection = 0;
    game->mode = GAME_MODE_SURVIVAL;
    game->attack_boss_defeated = false;

    srand((unsigned int)time(NULL));
    clear_enemies();
}

void game_tick(Game *game, double dt) {
    InputState in;
    input_poll(&in);

    if (in.quit) {
        game->running = false;
        return;
    }

    if (in.toggle_fullscreen) {
        video_toggle_fullscreen();
    }

    if (game->in_game_over) {
        if (in.menu_select) {
            game->in_game_over = false;
            game->in_menu = true;
            game->menu_selection = game->game_over_victory ? 1 : 0;
        }
        return;
    }

    if (game->in_menu) {
        if (in.menu_up || in.menu_down) {
            game->menu_selection = 1 - game->menu_selection;
        }
        if (in.menu_select) {
            start_mode(game, game->menu_selection == 0 ? GAME_MODE_SURVIVAL : GAME_MODE_ATTACK);
        }
        return;
    }

    game->survival_time += dt;

    if (s_shoot_flash_timer > 0.0) s_shoot_flash_timer -= dt;
    if (s_damage_flash_timer > 0.0) s_damage_flash_timer -= dt;
    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (s_enemies[i].hit_flash_timer > 0.0) {
            s_enemies[i].hit_flash_timer -= dt;
        }
    }

    if (in.reload && game->ammo < MAG_CAPACITY && game->reserve_ammo > 0) {
        int need = MAG_CAPACITY - game->ammo;
        int load = (game->reserve_ammo < need) ? game->reserve_ammo : need;
        game->ammo += load;
        game->reserve_ammo -= load;
    }

    s_spawn_timer += dt;
    if (game->mode == GAME_MODE_SURVIVAL) {
        while (s_spawn_timer >= s_spawn_interval) {
            s_spawn_timer -= s_spawn_interval;
            (void)try_spawn_enemy_random(game, ENEMY_MAX_HP, false, 3.0);
            s_spawn_interval *= SPAWN_ACCELERATION;
            if (s_spawn_interval < SPAWN_INTERVAL_MIN) {
                s_spawn_interval = SPAWN_INTERVAL_MIN;
            }
        }
    } else {
        while (s_spawn_timer >= s_spawn_interval && game->attack_spawned_regular < game->attack_target_kills) {
            s_spawn_timer -= s_spawn_interval;
            if (try_spawn_enemy_random(game, ENEMY_MAX_HP, false, 3.5)) {
                game->attack_spawned_regular++;
            }
        }

        if (!game->attack_boss_spawned &&
            game->kills >= game->attack_target_kills &&
            count_alive_regular_enemies() == 0) {
            if (try_spawn_enemy_random(game, BOSS_HP, true, 5.0)) {
                game->attack_boss_spawned = true;
            }
        }
    }

    if (in.shoot && game->ammo > 0) {
        game->ammo--;
        s_shoot_flash_timer = 0.12;
        double best_rel = 0.18;
        int best_idx = -1;

        for (int i = 0; i < ENEMY_CAPACITY; ++i) {
            if (!s_enemies[i].alive) continue;

            double dx = s_enemies[i].x - game->player.x;
            double dy = s_enemies[i].y - game->player.y;
            double dist = sqrt(dx * dx + dy * dy);
            if (dist < 0.5 || dist > game->max_distance) continue;

            double angle = atan2(dy, dx);
            double rel = angle - game->player.dir;
            while (rel > PI) rel -= 2.0 * PI;
            while (rel < -PI) rel += 2.0 * PI;
            if (fabs(rel) >= best_rel) continue;

            if (has_line_of_sight(game, game->player.x, game->player.y, s_enemies[i].x, s_enemies[i].y)) {
                best_rel = fabs(rel);
                best_idx = i;
            }
        }

        if (best_idx >= 0) {
            s_enemies[best_idx].hp--;
            s_enemies[best_idx].hit_flash_timer = 0.18;
            if (s_enemies[best_idx].hp <= 0) {
                bool was_boss = s_enemies[best_idx].boss;
                s_enemies[best_idx].alive = false;

                if (was_boss) {
                    game->attack_boss_defeated = true;
                } else {
                    game->kills++;
                    game->reserve_ammo += 2;
                }
            }
        }
    }

    const double mouse_sensitivity = 0.0008;
    const double arrow_turn_speed = 2.0;
    game->player.dir -= (double)in.mouse_delta_x * mouse_sensitivity;
    if (in.turn_left) game->player.dir -= arrow_turn_speed * dt;
    if (in.turn_right) game->player.dir += arrow_turn_speed * dt;

    while (game->player.dir > PI) game->player.dir -= 2.0 * PI;
    while (game->player.dir < -PI) game->player.dir += 2.0 * PI;

    double forward_x = cos(game->player.dir);
    double forward_y = sin(game->player.dir);
    double side_x = cos(game->player.dir + PI / 2.0);
    double side_y = sin(game->player.dir + PI / 2.0);

    double move_x = 0.0;
    double move_y = 0.0;

    if (in.move_forward) {
        move_x += forward_x;
        move_y += forward_y;
    }
    if (in.move_backward) {
        move_x -= forward_x;
        move_y -= forward_y;
    }
    if (in.strafe_left) {
        move_x -= side_x;
        move_y -= side_y;
    }
    if (in.strafe_right) {
        move_x += side_x;
        move_y += side_y;
    }

    double len = sqrt(move_x * move_x + move_y * move_y);
    if (len > 0.0001) {
        move_x /= len;
        move_y /= len;

        double nx = game->player.x + move_x * game->move_speed * dt;
        double ny = game->player.y + move_y * game->move_speed * dt;

        if (!map_is_wall(game, nx, game->player.y)) {
            game->player.x = nx;
        }
        if (!map_is_wall(game, game->player.x, ny)) {
            game->player.y = ny;
        }
    }

    const double regular_speed = 1.15;
    const double boss_speed = 0.9;
    int pending_damage = 0;

    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (!s_enemies[i].alive) {
            continue;
        }

        double dx = game->player.x - s_enemies[i].x;
        double dy = game->player.y - s_enemies[i].y;
        double d = sqrt(dx * dx + dy * dy);
        if (d < 0.001) {
            continue;
        }

        double stop_dist = s_enemies[i].boss ? 0.55 : 0.35;
        if (d > stop_dist) {
            double speed = s_enemies[i].boss ? boss_speed : regular_speed;
            double vx = dx / d;
            double vy = dy / d;

            double ex_next = s_enemies[i].x + vx * speed * dt;
            double ey_next = s_enemies[i].y + vy * speed * dt;

            if (!map_is_wall(game, ex_next, s_enemies[i].y)) {
                s_enemies[i].x = ex_next;
            }
            if (!map_is_wall(game, s_enemies[i].x, ey_next)) {
                s_enemies[i].y = ey_next;
            }
        }

        double hit_dist = s_enemies[i].boss ? 0.9 : 0.65;
        if (d < hit_dist) {
            int dmg = s_enemies[i].boss ? 14 : 8;
            if (dmg > pending_damage) {
                pending_damage = dmg;
            }
        }
    }

    if (s_contact_damage_cooldown > 0.0) {
        s_contact_damage_cooldown -= dt;
        if (s_contact_damage_cooldown < 0.0) {
            s_contact_damage_cooldown = 0.0;
        }
    }

    if (pending_damage > 0 && s_contact_damage_cooldown <= 0.0) {
        game->hp -= pending_damage;
        if (game->hp < 0) {
            game->hp = 0;
        }
        s_contact_damage_cooldown = 0.40;
        s_damage_flash_timer = 0.35;
    }

    if (game->hp <= 0) {
        s_last_final_kills = game->kills;
        s_last_final_time  = game->survival_time;
        game->in_game_over      = true;
        game->game_over_victory = false;
        return;
    }

    if (game->mode == GAME_MODE_ATTACK && game->attack_boss_defeated) {
        s_last_final_kills = game->kills;
        s_last_final_time  = game->survival_time;
        game->in_game_over      = true;
        game->game_over_victory = true;
    }
}

void game_render(const Game *game) {
    int w = game->screen_width;
    int h = game->screen_height;

    char *screen = (char *)malloc((size_t)(w * h));
    double *depth_buffer = (double *)malloc((size_t)w * sizeof(double));
    if (!screen || !depth_buffer) {
        free(screen);
        free(depth_buffer);
        return;
    }

    memset(screen, ' ', (size_t)(w * h));

    /* ---- Ecran de fin (mort ou victoire) ---- */
    if (game->in_game_over) {
        const char *headline = game->game_over_victory ? "VICTOIRE !" : "TU ES MORT";
        const char *mode_str = (game->mode == GAME_MODE_SURVIVAL) ? "MODE SURVIE" : "MODE ATTAQUE";

        int minutes = (int)(s_last_final_time / 60.0);
        int seconds  = (int)s_last_final_time % 60;

        char stat_line[160];
        snprintf(stat_line, sizeof(stat_line),
                 "KILLS: %d   TEMPS: %02d:%02d   %s",
                 s_last_final_kills, minutes, seconds, mode_str);

        const char *hint = "ENTREE: RETOUR AU MENU";

        int y = h / 2 - 3;
        int x = (w - (int)strlen(headline)) / 2;
        for (int i = 0; headline[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, headline[i]);

        y += 2;
        x = (w - (int)strlen(stat_line)) / 2;
        for (int i = 0; stat_line[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, stat_line[i]);

        y += 3;
        x = (w - (int)strlen(hint)) / 2;
        for (int i = 0; hint[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, hint[i]);

        video_clear();
        video_draw_ascii(screen, w, h);
        video_present();

        free(depth_buffer);
        free(screen);
        return;
    }

    if (game->in_menu) {
        const char *title = "COWDOOM";
        const char *subtitle = "CHOISIS UN MODE";
        const char *m0 = "MODE SURVIE";
        const char *m1 = "MODE ATTAQUE";
        const char *hint = "FLECHES HAUT/BAS + ENTREE";
        const char *desc0 = "SURVIE: VAGUES INFINIES QUI ACCELERENT";
        const char *desc1 = "ATTAQUE: MAP XXL, KILLS DEFINIS, BOSS FINAL";

        int y = h / 2 - 5;
        int x = (w - (int)strlen(title)) / 2;
        for (int i = 0; title[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, title[i]);
        y += 2;
        x = (w - (int)strlen(subtitle)) / 2;
        for (int i = 0; subtitle[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, subtitle[i]);
        y += 2;
        int y_mode0 = y;
        x = (w - (int)strlen(m0)) / 2;
        for (int i = 0; m0[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, m0[i]);
        y += 1;
        x = (w - (int)strlen(desc0)) / 2;
        for (int i = 0; desc0[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, desc0[i]);
        y += 2;
        int y_mode1 = y;
        x = (w - (int)strlen(m1)) / 2;
        for (int i = 0; m1[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, m1[i]);
        y += 1;
        x = (w - (int)strlen(desc1)) / 2;
        for (int i = 0; desc1[i] && x + i < w; ++i) put_char(screen, w, h, x + i, y, desc1[i]);

        /* Curseur de selection explicite a gauche de l'option active. */
        {
            int sel_y = (game->menu_selection == 0) ? y_mode0 : y_mode1;
            int sel_x = (w - (int)strlen(m0)) / 2 - 4;
            put_char(screen, w, h, sel_x + 0, sel_y, '>');
            put_char(screen, w, h, sel_x + 1, sel_y, '>');
            put_char(screen, w, h, sel_x + 2, sel_y, '>');
        }

        x = (w - (int)strlen(hint)) / 2;
        for (int i = 0; hint[i] && x + i < w; ++i) put_char(screen, w, h, x + i, h - 2, hint[i]);

        video_clear();
        video_draw_ascii(screen, w, h);
        video_present();

        free(depth_buffer);
        free(screen);
        return;
    }

    for (int y = 0; y < h; ++y) {
        char c;
        if (y < h / 2) {
            c = (y < h / 6) ? ' ' : '/';
        } else {
            c = (y > (h * 5) / 6) ? '.' : ',';
        }
        for (int x = 0; x < w; ++x) {
            screen[y * w + x] = c;
        }
    }

    for (int x = 0; x < w; ++x) {
        double cam = (2.0 * x / (double)w) - 1.0;
        double ray_angle = game->player.dir + cam * (game->fov * 0.5);
        double ray_x = cos(ray_angle);
        double ray_y = sin(ray_angle);

        int map_x = (int)game->player.x;
        int map_y = (int)game->player.y;

        double delta_x = (ray_x == 0.0) ? 1e30 : fabs(1.0 / ray_x);
        double delta_y = (ray_y == 0.0) ? 1e30 : fabs(1.0 / ray_y);

        int step_x, step_y;
        double side_dist_x, side_dist_y;

        if (ray_x < 0.0) {
            step_x = -1;
            side_dist_x = (game->player.x - map_x) * delta_x;
        } else {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - game->player.x) * delta_x;
        }
        if (ray_y < 0.0) {
            step_y = -1;
            side_dist_y = (game->player.y - map_y) * delta_y;
        } else {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - game->player.y) * delta_y;
        }

        int side = 0;
        bool hit = false;
        for (int step = 0; step < 96 && !hit; ++step) {
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_x;
                map_x += step_x;
                side = 0;
            } else {
                side_dist_y += delta_y;
                map_y += step_y;
                side = 1;
            }
            if (map_x < 0 || map_y < 0 ||
                map_x >= game->map.width || map_y >= game->map.height ||
                game->map.tiles[map_y * game->map.width + map_x] == '#') {
                hit = true;
            }
        }

        double perp_dist;
        if (side == 0) {
            perp_dist = ((double)map_x - game->player.x + (double)(1 - step_x) * 0.5) / ray_x;
        } else {
            perp_dist = ((double)map_y - game->player.y + (double)(1 - step_y) * 0.5) / ray_y;
        }

        if (perp_dist < 0.001) perp_dist = 0.001;
        if (!hit || perp_dist > game->max_distance) perp_dist = game->max_distance;

        depth_buffer[x] = perp_dist;

        int wall_h = (int)(h / perp_dist);
        int top = h / 2 - wall_h / 2;
        int bottom = h / 2 + wall_h / 2;
        if (top < 0) top = 0;
        if (bottom >= h) bottom = h - 1;

        char wc = shade_for_distance(perp_dist, game->max_distance);

        /* Face laterale un peu plus sombre pour mieux lire le relief. */
        if (side == 1) {
            wc = darken_wall_shade(wc);
        }

        /* Bord de cellule (jonction de murs) un peu plus sombre pour la lisibilite. */
        double hit_coord = (side == 0)
            ? (game->player.y + perp_dist * ray_y)
            : (game->player.x + perp_dist * ray_x);
        hit_coord -= floor(hit_coord);
        if (hit_coord < 0.08 || hit_coord > 0.92) {
            wc = darken_wall_shade(wc);
        }
        for (int y = top; y <= bottom; ++y) {
            char col = wc;
            if (y <= top + 1 || y >= bottom - 1) {
                col = darken_wall_shade(col);
            }
            screen[y * w + x] = col;
        }
    }

    draw_enemies(game, screen, depth_buffer);

    int mini_w = game->map.width;
    int mini_h = game->map.height;
    for (int my = 0; my < mini_h; ++my) {
        for (int mx = 0; mx < mini_w; ++mx) {
            char tile = game->map.tiles[my * mini_w + mx];
            put_char(screen, w, h, mx, my, tile == '#' ? '#' : ':');
        }
    }

    for (int i = 0; i < ENEMY_CAPACITY; ++i) {
        if (!s_enemies[i].alive) continue;
        put_char(screen, w, h, (int)s_enemies[i].x, (int)s_enemies[i].y, s_enemies[i].boss ? 'K' : 'E');
    }

    put_char(screen, w, h, (int)game->player.x, (int)game->player.y, 'C');

    char crosshair_c = (s_shoot_flash_timer > 0.0) ? '*' : '+';
    put_char(screen, w, h, w / 2, h / 2, crosshair_c);
    put_char(screen, w, h, w / 2 - 1, h / 2, '-');
    put_char(screen, w, h, w / 2 + 1, h / 2, '-');

    if (h >= 3) {
        char hud1[220];
        int filled = game->hp / 5;
        char bar[21];
        for (int i = 0; i < 20; ++i) {
            bar[i] = (i < filled) ? '#' : '-';
        }
        bar[20] = '\0';

        int alive_count = count_alive_enemies();
        int minutes = (int)(game->survival_time / 60.0);
        int seconds = (int)game->survival_time % 60;

        if (game->mode == GAME_MODE_SURVIVAL) {
            snprintf(hud1, sizeof(hud1),
                     "SURVIE  PV:[%s] %3d/100  MUN:%2d/%2d  RES:%3d  VACHES:%2d  KILLS:%3d  TEMPS:%02d:%02d",
                     bar, game->hp, game->ammo, MAG_CAPACITY, game->reserve_ammo,
                     alive_count, game->kills, minutes, seconds);
        } else {
            snprintf(hud1, sizeof(hud1),
                     "ATTAQUE  PV:[%s] %3d/100  MUN:%2d/%2d  RES:%3d  KILLS:%2d/%2d  BOSS:%s",
                     bar, game->hp, game->ammo, MAG_CAPACITY, game->reserve_ammo,
                     game->kills, game->attack_target_kills,
                     game->attack_boss_defeated ? "TOMBE" : (game->attack_boss_spawned ? "ACTIF" : "BLOQUE"));
        }

        const char *hud2 = "ZQSD: DEPLACEMENT  FLECHES/SOURIS: CAMERA  ESPACE/CLIC: TIR  R: RECHARGER";
        const char *hud3 = (game->mode == GAME_MODE_SURVIVAL)
            ? "OBJECTIF: SURVIVRE LE PLUS LONGTEMPS POSSIBLE  F11: PLEIN ECRAN  X/ESC: QUITTER"
            : "OBJECTIF: ELIMINER LES CIBLES PUIS LE BOSS FINAL  F11: PLEIN ECRAN  X/ESC: QUITTER";

        int len1 = (int)strlen(hud1);
        int len2 = (int)strlen(hud2);
        int len3 = (int)strlen(hud3);

        for (int i = 0; i < len1 && i < w; ++i) screen[(h - 3) * w + i] = hud1[i];
        for (int i = len1; i < w; ++i) screen[(h - 3) * w + i] = ' ';

        for (int i = 0; i < len2 && i < w; ++i) screen[(h - 2) * w + i] = hud2[i];
        for (int i = len2; i < w; ++i) screen[(h - 2) * w + i] = ' ';

        for (int i = 0; i < len3 && i < w; ++i) screen[(h - 1) * w + i] = hud3[i];
        for (int i = len3; i < w; ++i) screen[(h - 1) * w + i] = ' ';
    }

    video_clear();
    video_draw_ascii(screen, w, h);
    if (s_damage_flash_timer > 0.0) {
        video_draw_border_flash((float)(s_damage_flash_timer / 0.35));
    }
    video_present();

    free(depth_buffer);
    free(screen);
}

void game_shutdown(void) {
}
