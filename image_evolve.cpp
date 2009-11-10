#include "evolve.h"

using namespace std;

int main() {
	candidate_t *fittest;
	candidate_t *best;
	population_t *pop;
	pop = new population_t();

	printf("Generando primera poblacion...\n");
	pop->next_generation();
	best = pop->get_fittest();
	printf("  Fitness: %f %%\n", (double) best->fitness * 100);
	best->write();
	
	for (int count = 1; ; count++) {
		printf("Generando poblacion... [%d]\n", count);
		pop->next_generation();
		
		fittest = pop->get_fittest();
		if (fittest->fitness > best->fitness)
			best = fittest;

		printf("  Fitness: %f %%\n", (double) fittest->fitness * 100);
		printf("  Best:    %f %%\n", (double) best->fitness * 100);
		
		if (count % 10 == 0)
			fittest->write();
	}
	
	printf("  Best Fitness: %f %%\n", (double) best->fitness * 100);
	best->write();
}
