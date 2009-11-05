#include "evolve.h"

using namespace std;

int main() {
	candidate_t *fittest;
	population_t *pop;
	pop = new population_t();
	
	for (int count = 0; count <= 500; count++) {
		printf("Generando pr—xima poblaci—n... [%d]\n", count);
		pop->next_generation();
		
		fittest = pop->get_fittest();
		printf("  Fitness: %f %%\n", (double) fittest->fitness * 100);
		
		if (count % 5 == 0)
			fittest->write();
	}
}
