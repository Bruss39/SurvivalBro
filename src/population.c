#include "population.h"
#include "world.h"

#include <stdio.h>


// insert a bro into a small list ordered by age
static void keep_oldest(Entity oldest[3], size_t *const count, const Entity *const bro){
    size_t position = 0;
    while (position < *count && oldest[position].age >= bro->age) position++;
    if (position == 3) return;

    if (*count < 3) (*count)++;
    for (size_t i = *count - 1; i > position; i--) oldest[i] = oldest[i - 1];
    oldest[position] = *bro;
}


// avoid meaningless divisions when no observations are available
static void print_average(const char *const label, const double total, const size_t count){
    if (count == 0) printf("%s: N/A\n", label);
    else printf("%s: %.2f\n", label, total / (double)count);
}


Population population_init(void){
    Population population = {0};

    return population;
}


Entity *population_get_free(Population *const population){
    if (population == NULL) return NULL;

    for (size_t i = 0; i < WORLD_SIZE * WORLD_SIZE; i++) {
        if (!population->bros[i].alive) return &population->bros[i];
    }
    return NULL;
}


bool population_new_birth(Population *const population){
    if (population == NULL || population->alive >= WORLD_SIZE * WORLD_SIZE) return false;

    population->all++;
    population->alive++;

    return true;
}


void population_new_death(Population *const population, const Entity *const bro){
    if (population == NULL || bro == NULL || population->alive == 0) return;

    population->alive--;
    population->total_death_age += bro->age;
    keep_oldest(population->oldest, &population->oldest_count, bro);
}


void population_print(const Population *const population, const World *const world){
    if (population == NULL || world == NULL) return;

    size_t total_age = 0;
    size_t total_energy = 0;
    double total_move_probability = 0.0;
    Entity oldest[3] = {0};
    size_t oldest_count = population->oldest_count;
    for (size_t i = 0; i < oldest_count; i++) oldest[i] = population->oldest[i];

    for (size_t i = 0; i < WORLD_SIZE * WORLD_SIZE; i++) {
        const Entity *current = &population->bros[i];
        if (!current->alive) continue;

        total_age += current->age;
        total_energy += (size_t)current->energy;
        total_move_probability += current->genome.move_probability;
        keep_oldest(oldest, &oldest_count, current);
    }

    size_t deaths = population->all - population->alive;
    size_t births = population->all == 0 ? 0 : population->all - 1;
    size_t space = world->dimension * world->dimension - world->qty_obstacle;

    printf("\nFinal statistics\n");
    printf("Colony days: %zu\n", world->age);
    printf("Total bros (including the founder): %zu\n", population->all);
    printf("Alive: %zu | Births: %zu | Deaths: %zu\n", population->alive, births, deaths);
    printf("Available space: %zu | Food: %zu | Toxic: %zu\n", space, world->qty_food, world->qty_toxic);
    print_average("Alive / available space", (double)population->alive, space);
    print_average("Alive / food", (double)population->alive, world->qty_food);
    print_average("Alive / toxic", (double)population->alive, world->qty_toxic);
    print_average("Average age (alive)", (double)total_age, population->alive);
    print_average("Average energy (alive)", (double)total_energy, population->alive);
    print_average("Average age at death", (double)population->total_death_age, deaths);
    print_average("Average age at first son (parents)", (double)population->total_first_son_age, population->parents);
    print_average("Average energy before reproduction", (double)population->total_reproduction_energy, births);
    print_average("Average sons per bro (all)", (double)births, population->all);
    print_average("Average movement probability (alive)", total_move_probability, population->alive);

    printf("\nOldest bros (alive or dead)\n");
    for (size_t i = 0; i < oldest_count; i++) {
        const Entity *bro = &oldest[i];
        printf("Bro #%zu | age: %zu | born: %zu | ", bro->id, bro->age, bro->birth_day);
        if (bro->alive) printf("alive");
        else printf("died: %zu", bro->death_day);
        printf(" | sons: %zu\n", bro->son);
        printf("  move: %.3f | reproduce: %.3f | child energy: %.3f | metabolism: %.3f | mutation: %.3f\n",
               bro->genome.move_probability, bro->genome.reproduction_probability,
               bro->genome.child_energy_fraction, bro->genome.metabolism, bro->genome.mutation_rate);
    }
}
