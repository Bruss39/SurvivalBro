#include "spot.h"
#include "config.h"
#include "genome.h"
#include "entity.h"
#include "population.h"
#include "world.h"

#include <stdlib.h>


void spot_alteration(Entity *const bro, Population *const population, World *const world){
    if (bro == NULL || world == NULL || population == NULL) exit(EXIT_FAILURE);

    if (world->grid[bro->position.x][bro->position.y] == CELL_FOOD){
        bro->energy += genome_metabolize(bro, FOOD_ALTERATION);
    }
    else if (world->grid[bro->position.x][bro->position.y] == CELL_TOXIC){
        bro->energy += genome_metabolize(bro, TOXIC_ALTERATION);
    }
    else return;

    // food and toxic are consumed when reached
    world_set_cell(world, bro->position, CELL_EMPTY);

    // check if still alive
    if (bro->energy <= 0) entity_kill(bro, population, world);
}
