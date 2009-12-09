#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define POP_SIZE 36 // Múltiplo de NUM_CHILDREN y múltiplo de 6 (SPEs)
#define NUM_POLY 48 // Múltiplo de 8 ((POLY_LENGTH * NUM_POLY) % 128 == 0)
#define NUM_VERT 6
#define POLY_LENGTH (4 + NUM_VERT * 2)
#define DNA_LENGTH  (NUM_POLY * POLY_LENGTH)

#define MUTATE_CHANCE .02
#define MUTATE_AMOUNT .1
#define NUM_CHILDREN  6
#define NUM_COOL_PARENTS (POP_SIZE / NUM_CHILDREN)
#define RGB_BITS (MaxRGB == 255 ? 0 : 8)
#define RAND ((double) (rand() % 10000 / 10000.))

typedef struct {
  float *sol;
  float *p1;
  float *p2;
  char dummy[116];
} pair_parents_t;

