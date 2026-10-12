# SurvivalBro

A small artificial life simulation written in C. A colony lives on a 2D grid,
consumes energy, reproduces and dies. Children inherit their parent's genome,
with a chance of random mutations. The goal is to observe how simple inherited
behaviours affect survival in an environment with limited resources.

The simulation runs in the terminal and uses no external libraries.

## Build and run

You need GCC with C11 support. Make is optional. Windows and Linux are supported.

```sh
make
```

Without Make:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude src/main.c src/entity.c src/genome.c src/population.c src/spot.c src/world.c -o survivalbro
```

On Windows, use `-o survivalbro.exe` for the direct GCC command. Run from PowerShell:

```powershell
.\survivalbro.exe
.\survivalbro.exe --seed 23 --days 100 --fast --quiet
```

On Linux:

```sh
./survivalbro
./survivalbro --seed 23 --days 100 --fast --quiet
```

| Option | Effect |
| --- | --- |
| `--seed N` | Set the random seed; default: current time. |
| `--days N` | Stop after N days; default: 300. |
| `--fast` | Skip the one-second pause between days. |
| `--quiet` | Show only the final report. |
| `--help` | Show usage. |

With the same executable, seed and settings, a run is repeatable. Different seeds
can lead to early extinction or several generations. A run ends when the colony
is extinct or reaches the day limit.

## Rules

- `X`: bro, `.`: empty, `o`: obstacle, `f`: food, `t`: toxic.
- Bros are printed in black on a yellow background in supported terminals, with the colour reset immediately after each `X`. Redirected output, unsupported terminals and `NO_COLOR` use plain text.
- Each day, empty spots have a 3% chance of growing food and a 1% chance of becoming toxic.
- Each bro can move to a random adjacent spot, excluding obstacles and other bros.
- Movement costs 4 energy. Each daily turn costs another 2 energy.
- Food gives up to 30 energy; toxic removes up to 10. Both are scaled by the bro's metabolism and consumed on contact, including at birth.
- From day 2, a bro with at least 50 energy can reproduce according to its reproduction probability, transferring a fraction of its energy to a child in a free adjacent spot. Both must receive positive energy.
- Each child inherits five traits: movement probability, reproduction probability, child energy fraction, metabolism and mutation rate. Each trait can independently mutate to a random value in [0, 1].
- Newborns act starting the next day. A bro dies when its energy reaches zero.

All defaults are in `include/config.h`, including the founder's genome. Metabolism
controls absorption of food and toxic; daily energy loss is fixed. The food value
and starting genome are chosen to allow more than immediate starvation.

The population keeps at most `WORLD_SIZE * WORLD_SIZE` living bros in a fixed
array. Dead slots are reused, while lifetime birth/death statistics and the three
oldest bros are preserved. Total births can therefore exceed the array size.

## Report and project structure

The final report shows colony duration, births, deaths, resource ratios, average
age and energy of survivors, age at death and first child, reproduction energy,
sons per bro, movement probability and the genomes of the three oldest bros.
Values without observations are shown as `N/A`. Ratios use the living population;
available space means all non-obstacle cells.

The original module layout is retained: `world` handles the grid, `spot` handles
resources, `entity` handles individual actions, `genome` handles inherited traits,
`population` handles slots and statistics, and `main` runs the daily loop.

Program sources are in `src/`, header files are in `include/`, and local
regression tests are in `tests/`. The Makefile and generated executables remain
in the project root.
