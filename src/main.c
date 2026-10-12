#include "config.h"
#include "entity.h"
#include "genome.h"
#include "world.h"
#include "population.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


static bool parse_number(const char *const text, unsigned long *const number){
    if (text == NULL || text[0] < '0' || text[0] > '9') return false;

    char *end;
    errno = 0;
    *number = strtoul(text, &end, 10);
    return errno == 0 && *end == '\0';
}


static void wait_day(void){
#ifdef _WIN32
    Sleep((DWORD)DELAY_BETWEEN_DAYS * 1000);
#else
    sleep(DELAY_BETWEEN_DAYS);
#endif
}


int main(int argc, char **argv) {
    unsigned int seed = (unsigned int)time(NULL);
    size_t max_days = MAX_DAYS;
    bool fast = false;
    bool quiet = false;

    // small optional controls for repeatable experiments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--fast") == 0) fast = true;
        else if (strcmp(argv[i], "--quiet") == 0) quiet = true;
        else if (strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [--seed N] [--days N] [--fast] [--quiet]\n", argv[0]);
            return EXIT_SUCCESS;
        }
        else if (strcmp(argv[i], "--seed") == 0 || strcmp(argv[i], "--days") == 0) {
            bool is_seed = strcmp(argv[i], "--seed") == 0;
            unsigned long number;
            if (++i >= argc || !parse_number(argv[i], &number) ||
                (is_seed && number > UINT_MAX) || (!is_seed && number == 0)) {
                fprintf(stderr, "Invalid value for --%s.\n", is_seed ? "seed" : "days");
                return EXIT_FAILURE;
            }
            if (is_seed) seed = (unsigned int)number;
            else max_days = (size_t)number;
        }
        else {
            fprintf(stderr, "Unknown option: %s. Use --help.\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

    srand(seed);
    printf("SurvivalBro | seed: %u\n", seed);

    // world
    World world = world_init(WORLD_SIZE);
    world_generation(&world, WORLD_DENSITY);

    // population
    Population population = population_init();

    // first bro
    Entity *first_bro = &population.bros[0];
    if (!entity_init(first_bro, &world, &population)) {
        fprintf(stderr, "No free spot for the first bro.\n");
        world_free(&world);
        return EXIT_FAILURE;
    }

    if (!quiet) {
        printf("X bro | . empty | o obstacle | f food | t toxic\n");
        printf("Day 0 | alive: %zu\n", population.alive);
        world_print(&world);
    }
    

    // run world
    while (population.alive != 0 && world.age < max_days){
        world.age++;
        world_regenerate(&world);

        for (size_t i = 0; i < WORLD_SIZE * WORLD_SIZE; i++){
            Entity *current = &population.bros[i];
            // newborns start acting the next day, even in a reused slot
            if (!current->alive || current->birth_day == world.age) continue;
            current->age++;

            // movement
            if (current->alive && genome_can_move(current)){
                entity_move(current, &world, &population);
            }

            // reproduction (not allowed the first day)
            if (current->alive && world.age > 1 && genome_can_reproduce(current)){
                entity_duplicate(current, &world, &population);
            }

            // apply daily energy loss
            if (current->alive) {
                current->energy -= DAILY_LOSS;
                if (current->energy <= 0) entity_kill(current, &population, &world);
            }
        }
        // show the current state of the world
        if (!quiet) {
            printf("Day %zu | alive: %zu | total bros: %zu | food: %zu | toxic: %zu\n",
                   world.age, population.alive, population.all, world.qty_food, world.qty_toxic);
            world_print(&world);
        }

        // wait before the next day
        if (!fast && population.alive != 0 && world.age < max_days) wait_day();
    }

    printf("\nSimulation ended: %s.\n", population.alive == 0 ? "colony extinct" : "day limit reached");
    population_print(&population, &world);
    world_free(&world);
    return EXIT_SUCCESS;
}
