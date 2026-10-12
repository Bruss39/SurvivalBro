#ifndef CONFIG_H
#define CONFIG_H

/* Time */
#define DELAY_BETWEEN_DAYS 1         // in sec
#define MAX_DAYS 300

/* World */
#define WORLD_SIZE 10
#define WORLD_DENSITY 40
#define FOOD_REGEN_CHANCE 2          // percent per empty cell, each day
#define TOXIC_REGEN_CHANCE 1

/* Entity */
#define ENTITY_START_ENERGY 70
#define MIN_ENERGY_TO_REPRODUCE 50
#define MOVEMENT_COST 4              // movement cost in energy
#define DAILY_LOSS 2                 // energy loss at the end of the day

/* Cell alteration */
#define TOXIC_ALTERATION -10
#define FOOD_ALTERATION +30

/* Genome first bro */
#define START_MOVE_PROBABILITY 0.65f
#define START_REPRODUCTION_PROBABILITY 0.35f
#define START_CHILD_ENERGY_FRACTION 0.35f
#define START_METABOLISM 0.90f
#define START_MUTATION_RATE 0.10f

#if WORLD_SIZE <= 0 || WORLD_DENSITY < 0 || WORLD_DENSITY > 100
#error Invalid world configuration
#endif

#if FOOD_REGEN_CHANCE < 0 || TOXIC_REGEN_CHANCE < 0 || FOOD_REGEN_CHANCE + TOXIC_REGEN_CHANCE > 100
#error Invalid regeneration probabilities
#endif

#if ENTITY_START_ENERGY <= 0 || MIN_ENERGY_TO_REPRODUCE <= 0 || MOVEMENT_COST < 0 || DAILY_LOSS <= 0
#error Invalid energy configuration
#endif

#if MAX_DAYS <= 0 || DELAY_BETWEEN_DAYS < 0
#error Invalid time configuration
#endif

#endif
