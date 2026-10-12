#ifndef GENOME_H
#define GENOME_H

#include <stdbool.h>


typedef struct Entity Entity;  // forward declaration

typedef struct Genome{
    float move_probability;        // all genome characteristics are between 0.0f and 1.0f
    float reproduction_probability;
    float child_energy_fraction;
    float metabolism;
    float mutation_rate;
} Genome;


// returns the first bro's genome from config.h
Genome genome_init(void);

// decides if the bro is fertile
bool genome_can_reproduce(const Entity *const bro);

// decides if the bro can move
bool genome_can_move(const Entity *const bro);

// decides how much energy to give to the son
int genome_son_energy(const Entity *const bro);

// returns the actual amount of energy absorbed
int genome_metabolize(const Entity *const bro, const int energy);

// returns an inherited genome with mutations based on bro's mutation rate
Genome genome_mutation(const Entity *const bro);

#endif
