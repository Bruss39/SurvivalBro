#include "entity.h"
#include "world.h"
#include "population.h"
#include "spot.h"
#include "config.h"
#include "coordinates.h"
#include "genome.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>


// spawn the bro in the world
static void spawn(const Entity *const bro, World *const world){
    world_set_cell(world, bro->position, CELL_BRO);
}


// choose a free, passable spot without retrying forever in a full world
static bool get_spawn(const World *const world, Coordinates *const position){
    size_t available = 0;
    for (size_t x = 0; x < world->dimension; x++) {
        for (size_t y = 0; y < world->dimension; y++) {
            if (world->grid[x][y] != CELL_OBSTACLE && world->grid[x][y] != CELL_BRO) available++;
        }
    }
    if (available == 0) return false;

    size_t selected = (size_t)rand() % available;
    for (size_t x = 0; x < world->dimension; x++) {
        for (size_t y = 0; y < world->dimension; y++) {
            if (world->grid[x][y] == CELL_OBSTACLE || world->grid[x][y] == CELL_BRO) continue;
            if (selected == 0) {
                *position = (Coordinates){(int)x, (int)y};
                return true;
            }
            selected--;
        }
    }
    return false;
}


// get a random coordinate of  an empty cell nearby the bro,
// returns false if there aren't any free cell
static bool get_nearby(const Entity *const bro, const World *const world, Coordinates *const nearby){
    if (bro == NULL || world == NULL) exit(EXIT_FAILURE);

    Coordinates candidates_moves[4];
    Coordinates moves[4];
    size_t moves_size = 0;
    
    // set candidates
    candidates_moves[0] = (Coordinates){bro->position.x +1, bro->position.y};
    candidates_moves[1] = (Coordinates){bro->position.x -1, bro->position.y};
    candidates_moves[2] = (Coordinates){bro->position.x, bro->position.y +1};
    candidates_moves[3] = (Coordinates){bro->position.x, bro->position.y -1};

    // filters candidates
    for (size_t i = 0; i < 4; i++){
        int x = candidates_moves[i].x;
        int y = candidates_moves[i].y;

        if (x >= 0 &&
            y >= 0 &&
            (size_t)x < world->dimension &&
            (size_t)y < world->dimension &&
            world->grid[x][y] != CELL_BRO &&
            world->grid[x][y] != CELL_OBSTACLE) {

            moves[moves_size] = candidates_moves[i];
            moves_size++;
        }
    }
    
    if (moves_size == 0) return false;

    *nearby = moves[(size_t)rand()%moves_size];
    return true;
}


bool entity_init(Entity *const bro, World *const world, Population *const population){
    if (bro == NULL || world == NULL|| population == NULL) exit(EXIT_FAILURE);
    if (bro->alive) return false;

    Coordinates position;
    if (!get_spawn(world, &position)) return false;
    if (!population_new_birth(population)) return false;

    // init bro
    *bro = (Entity){0};
    bro->alive = true;
    bro->id = population->all;
    bro->birth_day = world->age;
    bro->energy = ENTITY_START_ENERGY;
    bro->position = position;
    bro->genome = genome_init();

    spot_alteration(bro, population, world);
    if (bro->alive) spawn(bro, world);
    return true;
}


bool entity_move(Entity *const bro, World *const world, Population *const population){
    if (bro == NULL || world == NULL || population == NULL) exit(EXIT_FAILURE);
    if (!bro->alive) return false;

    Coordinates new_pos;
    if (!get_nearby(bro, world, &new_pos)) return false;  // can't move

    // move bro
    world_set_cell(world, bro->position, CELL_EMPTY);
    bro->position.x = new_pos.x;
    bro->position.y = new_pos.y;

    // apply any alteration
    spot_alteration(bro, population, world);
    if (!bro->alive) return true;

    // apply movement cost
    bro->energy -= MOVEMENT_COST;
    if (bro->energy <= 0) entity_kill(bro, population, world);

    // show bro if still alive
    if (bro->alive) spawn(bro, world);

    return true;
}


bool entity_duplicate(Entity *const bro, World *const world, Population *const population){
    if (bro == NULL || world == NULL || population == NULL) exit(EXIT_FAILURE);
    if (!bro->alive || bro->energy < MIN_ENERGY_TO_REPRODUCE) return false;

    Entity *son = population_get_free(population);
    if (son == NULL) return false;

    int son_energy = genome_son_energy(bro);
    if (son_energy <= 0 || son_energy >= bro->energy) return false;

    // get son coordinates
    Coordinates son_position;
    if (!get_nearby(bro, world, &son_position)) return false;  // can't duplicate

    if (!population_new_birth(population)) return false;

    // record reproduction before splitting the energy
    population->total_reproduction_energy += (size_t)bro->energy;
    if (bro->son == 0) {
        bro->age_at_first_son = bro->age;
        population->parents++;
        population->total_first_son_age += bro->age;
    }
    bro->son++;
    bro->energy -= son_energy;

    // set the son in a reusable population slot
    *son = (Entity){0};
    son->alive = true;
    son->id = population->all;
    son->birth_day = world->age;
    son->energy = son_energy;
    son->genome = genome_mutation(bro);
    son->position = son_position;

    // apply any alteration
    spot_alteration(son, population, world);
    
    if (son->alive) spawn(son, world);

    return true;
}


void entity_kill(Entity *const bro, Population *const population, World *const world){
    if (population == NULL || world == NULL || bro == NULL) exit(EXIT_FAILURE);
    if (bro->alive == false) return;  // already dead

    // set as dead
    bro->alive = false;
    bro->energy = 0;
    bro->death_day = world->age;

    // remove from the world
    world_set_cell(world, bro->position, CELL_EMPTY);

    population_new_death(population, bro);
}
