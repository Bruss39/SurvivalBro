#ifndef SPOT_H
#define SPOT_H

typedef struct Entity Entity;
typedef struct Population Population;
typedef struct World World;

typedef enum {
    CELL_BRO      = 'X',
    CELL_EMPTY    = '.',
    CELL_OBSTACLE = 'o',
    CELL_TOXIC    = 't',
    CELL_FOOD     = 'f'
} CellType;


// alter the bro based on the spot and metabolism
void spot_alteration(Entity *const bro, Population *const population, World *const world);
#endif
