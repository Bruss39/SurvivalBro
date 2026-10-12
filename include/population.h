#ifndef POPULATION_H
#define POPULATION_H

#include "config.h"
#include "entity.h"

#include <stddef.h>


typedef struct Population{
    Entity bros[WORLD_SIZE*WORLD_SIZE];
    size_t all;    // all bros (alive or dead)
    size_t alive;  // only alive bros
    size_t total_death_age;
    size_t parents;
    size_t total_first_son_age;
    size_t total_reproduction_energy;
    Entity oldest[3];     // oldest dead bros, kept when slots are reused
    size_t oldest_count;
} Population;

// init population with WORLD_SIZE^2 reusable slots
Population population_init(void);

// find an unused slot or reuse a dead bro's slot
Entity *population_get_free(Population *const population);

// simply increment a number of entity in the population
bool population_new_birth(Population *const population);

// update death statistics before a slot can be reused
void population_new_death(Population *const population, const Entity *const bro);

// print the final statistics, including the three oldest bros
void population_print(const Population *const population, const World *const world);

#endif
