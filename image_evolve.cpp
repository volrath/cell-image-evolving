#include "evolve.h"

int main() {
	candidate_t *fittest;
	population_t *pop;
	pop = new population_t();
	double best_fitness = .0;
	
	for (int count = 0;; count++) {
		printf("[%d] Generando poblacion...\n", count);
		pop->next_generation();
		
		fittest = pop->get_fittest();
		if (fittest->fitness > best_fitness)
			best_fitness = fittest->fitness;

		printf("  Fitness: %f %%\n", (double) fittest->fitness * 100);
		printf("  Best:    %f %%\n", (double) best_fitness * 100);
		
		if (count % 5 == 0)
			fittest->write();
	}
}
