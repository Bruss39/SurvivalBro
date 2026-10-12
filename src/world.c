#include "world.h"
#include "spot.h"
#include "config.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <string.h>
#include <unistd.h>


// keep unknown or monochrome terminals, and redirected output, plain
static bool terminal_colors(void){
    const char *term = getenv("TERM");
    const char *color_term = getenv("COLORTERM");
    const char *no_color = getenv("NO_COLOR");
    if (!isatty(STDOUT_FILENO) || (no_color != NULL && no_color[0] != '\0') || term == NULL ||
        strcmp(term, "dumb") == 0 || strstr(term, "mono") != NULL) return false;

    return (color_term != NULL && color_term[0] != '\0') ||
           strstr(term, "color") != NULL || strcmp(term, "linux") == 0 ||
           strcmp(term, "ansi") == 0 || strncmp(term, "xterm", 5) == 0 ||
           strncmp(term, "screen", 6) == 0 || strncmp(term, "tmux", 4) == 0 ||
           strncmp(term, "rxvt", 4) == 0;
}
#endif


World world_init(const size_t size) {
    if (size == 0 || size > INT_MAX) exit(EXIT_FAILURE);
    World world = {0};

    // init grid
    world.grid = malloc(size * sizeof(char *));
    if (world.grid == NULL) exit(EXIT_FAILURE);

    for (size_t i = 0; i < size; i++) {
        world.grid[i] = malloc(size * sizeof(char));

        if (world.grid[i] == NULL) {
            for (size_t k = 0; k < i; k++) {
                free(world.grid[k]);
            }

            free(world.grid);
            world.grid = NULL;
            exit(EXIT_FAILURE);
        }
    }

    // set all spot to '.'
    for (size_t i = 0; i < size; i++) {
        for (size_t j = 0; j < size; j++) {
            world.grid[i][j] = '.';
        }
    }

    // save
    world.dimension = size;
    world.qty_empty = size * size;

    return world;
}


bool world_generation(World *ptr, const size_t density) {
    if (ptr == NULL || ptr->grid == NULL) return false;
    if (density > 100) return false;

    for (size_t i = 0; i < ptr->dimension; i++) {
        for (size_t j = 0; j < ptr->dimension; j++) {
            if (ptr->grid[i][j] == CELL_BRO) continue;
            Coordinates position = {(int)i, (int)j};
            size_t r = (size_t)rand() % 100;

            if (r >= density) {
                world_set_cell(ptr, position, CELL_EMPTY);
            } else {
                size_t type = (size_t)rand() % 3;

                switch (type) {
                    case 0:
                        world_set_cell(ptr, position, CELL_OBSTACLE);
                        break;
                    case 1:
                        world_set_cell(ptr, position, CELL_TOXIC);
                        break;
                    case 2:
                        world_set_cell(ptr, position, CELL_FOOD);
                        break;
                }
            }
        }
    }

    return true;
}


void world_set_cell(World *const ptr, const Coordinates position, const CellType type){
    if (ptr == NULL || ptr->grid == NULL ||
        position.x < 0 || position.y < 0 ||
        (size_t)position.x >= ptr->dimension || (size_t)position.y >= ptr->dimension) exit(EXIT_FAILURE);

    // remove the old spot from the counters
    switch (ptr->grid[position.x][position.y]) {
        case CELL_EMPTY: ptr->qty_empty--; break;
        case CELL_OBSTACLE: ptr->qty_obstacle--; break;
        case CELL_FOOD: ptr->qty_food--; break;
        case CELL_TOXIC: ptr->qty_toxic--; break;
    }

    ptr->grid[position.x][position.y] = (char)type;

    // count the new spot (bros are counted by Population)
    switch (type) {
        case CELL_EMPTY: ptr->qty_empty++; break;
        case CELL_OBSTACLE: ptr->qty_obstacle++; break;
        case CELL_FOOD: ptr->qty_food++; break;
        case CELL_TOXIC: ptr->qty_toxic++; break;
        case CELL_BRO: break;
    }
}


void world_regenerate(World *const ptr){
    if (ptr == NULL || ptr->grid == NULL) return;

    for (size_t i = 0; i < ptr->dimension; i++) {
        for (size_t j = 0; j < ptr->dimension; j++) {
            if (ptr->grid[i][j] != CELL_EMPTY) continue;
            Coordinates position = {(int)i, (int)j};
            int r = rand() % 100;

            if (r < FOOD_REGEN_CHANCE) world_set_cell(ptr, position, CELL_FOOD);
            else if (r < FOOD_REGEN_CHANCE + TOXIC_REGEN_CHANCE) world_set_cell(ptr, position, CELL_TOXIC);
        }
    }
}


bool world_free(World *ptr) {
    if (ptr == NULL || ptr->grid == NULL) return false;

    for (size_t i = 0; i < ptr->dimension; i++) {
        free(ptr->grid[i]);
    }

    free(ptr->grid);
    ptr->grid = NULL;
    ptr->dimension = 0;
    ptr->age = 0;
    ptr->qty_empty = 0;
    ptr->qty_obstacle = 0;
    ptr->qty_food = 0;
    ptr->qty_toxic = 0;

    return true;
}


void world_print(const World *const ptr){
#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO state;
    const char *no_color = getenv("NO_COLOR");
    bool color = (no_color == NULL || no_color[0] == '\0') && GetConsoleScreenBufferInfo(console, &state) != 0;
#else
    bool color = terminal_colors();
#endif

    printf("\n");

    for (size_t i = 0; i < ptr->dimension; i++) {
        printf("[");

        for (size_t j = 0; j < ptr->dimension; j++) {
            if (color && ptr->grid[i][j] == CELL_BRO) {
                printf(" ");
#ifdef _WIN32
                // flush before each attribute change so only the X is highlighted
                fflush(stdout);
                bool colored = SetConsoleTextAttribute(console, BACKGROUND_RED | BACKGROUND_GREEN) != 0;
                printf("%c", ptr->grid[i][j]);
                fflush(stdout);
                if (colored) SetConsoleTextAttribute(console, state.wAttributes);
#else
                printf("\033[30;43m%c\033[0m", ptr->grid[i][j]);
#endif
                printf(" ");
            }
            else printf(" %c ", ptr->grid[i][j]);
        }
        printf("]\n");
    }

    printf("\n");
}
