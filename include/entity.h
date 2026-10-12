#ifndef ENTITY_H
#define ENTITY_H

#include "genome.h"
#include "coordinates.h"

#include <stddef.h>
#include <stdbool.h>


typedef struct Population Population;  // forward declaration
typedef struct World World;  // forward declaration

typedef struct Entity{
    Genome genome;
    Coordinates position;
    size_t id;
    size_t birth_day;
    size_t death_day;
    size_t age;
    size_t son;
    size_t age_at_first_son;
    int energy;
    bool alive;
} Entity;


// init an unused entity slot and spawn it on a free, passable spot
// returns false if there isn't any available spot
bool entity_init(Entity *const bro, World *const world, Population *const population);

// move the bro in a random nearby cell
// returns false if there aren't any free cell nearby the bro
bool entity_move(Entity *const bro, World *const world, Population *const population);

// duplicate the bro, split father's energy and spawn it;
// returns false if space or energy is insufficient
bool entity_duplicate(Entity *const bro, World *const world, Population *const population);

// set bro as dead
void entity_kill(Entity *const bro, Population *const population, World *const world);

#endif
