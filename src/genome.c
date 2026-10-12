#include "genome.h"
#include "config.h"
#include "entity.h"

#include <stdbool.h>
#include <stdlib.h>


// return a random number in [0, 1), so probabilities 0 and 1 work exactly
static double rand_float(void){
    double f = (double)rand() / ((double)RAND_MAX + 1.0);
    return f;
}


Genome genome_init(void){
    Genome gen;

    gen.move_probability = START_MOVE_PROBABILITY;
    gen.reproduction_probability = START_REPRODUCTION_PROBABILITY;
    gen.child_energy_fraction = START_CHILD_ENERGY_FRACTION;
    gen.metabolism = START_METABOLISM;
    gen.mutation_rate = START_MUTATION_RATE;
        
    return gen;
}

bool genome_can_reproduce(const Entity *const bro){
    if (bro == NULL || !bro->alive) return false;

    if (bro->energy < MIN_ENERGY_TO_REPRODUCE) return false;

    return rand_float() < bro->genome.reproduction_probability;
}

bool genome_can_move(const Entity *const bro){
    if (bro == NULL || !bro->alive) return false;

    return rand_float() < bro->genome.move_probability;
}

int genome_son_energy(const Entity *const bro){
    if (bro == NULL) exit(EXIT_FAILURE);

    return (int)((float)bro->energy * bro->genome.child_energy_fraction);
}

int genome_metabolize(const Entity *const bro, const int energy){
    if (bro == NULL) exit(EXIT_FAILURE);

    return (int)((float)energy * bro->genome.metabolism);
}

Genome genome_mutation(const Entity *const bro){
    if (bro == NULL) exit(EXIT_FAILURE);

    Genome new_gen = bro->genome;

    // mutates the new genoma
    if (rand_float() < bro->genome.mutation_rate) new_gen.move_probability = (float)rand_float();
    if (rand_float() < bro->genome.mutation_rate) new_gen.reproduction_probability = (float)rand_float();
    if (rand_float() < bro->genome.mutation_rate) new_gen.child_energy_fraction = (float)rand_float();
    if (rand_float() < bro->genome.mutation_rate) new_gen.metabolism = (float)rand_float();
    if (rand_float() < bro->genome.mutation_rate) new_gen.mutation_rate = (float)rand_float();

    return new_gen;
}
