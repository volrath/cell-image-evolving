#include "evolve.h"

int main(int argc, char** argv) {
	candidate_t *fittest;
	population_t *pop;
	char* in_file;
	double best_fitness = 0.;

	in_file = "media/firefox.jpg";
	pop = new population_t(argc == 1 ? in_file : argv[1]);
	
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
