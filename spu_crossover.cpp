#include <spu_mfcio.h>
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
  float *p1;
  float *p2;
  float *sol;
  char dummy[116];
} pair_parents_t;

pair_parents_t parents __attribute__ ((aligned(128)));
float p1[768] __attribute__ ((aligned(128)));
float p2[768] __attribute__ ((aligned(128)));
float sol[768] __attribute__ ((aligned(128)));

int main(unsigned long long speid, unsigned long long argp, unsigned long long  envp) {
  unsigned int spu_id = spu_read_in_mbox();
  int tag = 1, tag_mask = 1 << tag;

  mfc_get(&parents, (unsigned int) argp, envp, tag, 0, 0);
  mfc_write_tag_mask(tag_mask);
  mfc_read_tag_status_all();

  mfc_get(p1, (unsigned long int) parents.p1, sizeof(p1), tag, 0, 0);
  mfc_read_tag_status_all();

  mfc_get(p2, (unsigned long int) parents.p2, sizeof(p2), tag, 0, 0);
  mfc_read_tag_status_all();

  printf("spu #%d parents: %f %f\n", spu_id, p1[0], p2[0]);

  float *act, val;
  for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
    act = (RAND < 0.5) ? p1 : p2;

    for (int j = 0; j < POLY_LENGTH; j++) {
      val = act[i+j];
      if (RAND < MUTATE_CHANCE) {
        val += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
        if (val < 0.) val = 0.;
        if (val > 1.) val = 1.;
      }
      sol[i+j] = val;
    }
  }

  printf("%d %f\n", sizeof(sol), sol[0]);
  printf("%x %x\n", sol, parents.sol);
  mfc_put(sol, (unsigned long int) parents.sol, sizeof(sol), tag, 0, 0);
  mfc_read_tag_status_all();
  printf("shit\n");
  
  return 0;
}
