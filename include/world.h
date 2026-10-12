#ifndef WORLD_H
#define WORLD_H

#include "coordinates.h"
#include "spot.h"

#include <stddef.h>
#include <stdbool.h>


typedef struct World{
    size_t dimension;
    size_t qty_obstacle;
    size_t qty_food;
    size_t qty_toxic;
    size_t qty_empty;
    size_t age;        // age of the world in days
    char **grid;
} World;

// set all spots to '.'
World world_init(const size_t size);

// free world's mem
bool world_free(World *ptr);

// fills the world considering density
// density range = 0-100
bool world_generation(World *ptr, const size_t density);

// replace a spot and keep the world counters updated
void world_set_cell(World *const ptr, const Coordinates position, const CellType type);

// regenerate food and toxic on empty spots only
void world_regenerate(World *const ptr);

// print the world in terminal
void world_print(const World *const ptr);

#endif
