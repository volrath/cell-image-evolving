#include "defines.h"
#include <spu_mfcio.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

pair_parents_t parents __attribute__ ((aligned(128)));
float p1[DNA_LENGTH]   __attribute__ ((aligned(128)));
float p2[DNA_LENGTH]   __attribute__ ((aligned(128)));
float sol[DNA_LENGTH]  __attribute__ ((aligned(128)));

int main(unsigned long long speid, unsigned long long argp, unsigned long long envp) {
	srand(time(NULL));
	unsigned int spu_id = spu_read_in_mbox();
	int tag = 1, tag_mask = 1 << tag;

	mfc_get(&parents, (unsigned int) argp, envp, tag, 0, 0);
	mfc_write_tag_mask(tag_mask);
	mfc_read_tag_status_all();

	mfc_get(p1, (unsigned long int) parents.p1, sizeof(p1), tag, 0, 0);
	mfc_read_tag_status_all();

	mfc_get(p2, (unsigned long int) parents.p2, sizeof(p2), tag, 0, 0);
	mfc_read_tag_status_all();

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

	mfc_put(sol, (unsigned long int) parents.sol, sizeof(sol), tag, 0, 0);
	mfc_read_tag_status_all();

	return 0;
}
