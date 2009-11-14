#include "evolve.h"

using namespace std;

int main(int argc, char** argv) {
	candidate_t *fittest;
	population_t *pop;
	int it;
	char* in_file;
	double best_fitness = 0.;

	it = 100000;
	in_file = "media/firefox.jpg";
	
	for (int i = 1; i < argc; i += 2) {
		char* opc;
		opc = argv[i];

		if (strcmp(opc, "-it") == 0)
			it = atoi(argv[i+1]);
		else if (strcmp(opc, "-file") == 0)
			in_file = argv[i+1];
		else
			fprintf(stderr, "Error: Comando no existente\n");
	}
	
	pop = new population_t(in_file);
	for (int count = 0; count < it; count++) {
		printf("[%d] Generando poblacion...\n", count);
		pop->next_generation();
		
		fittest = pop->get_fittest();
		if (fittest->fitness > best_fitness) {
		  fittest->write();
		  best_fitness = fittest->fitness;
		}

		printf("  Fitness: %f %%\n", (double) fittest->fitness * 100);
		printf("  Best:    %f %%\n", (double) best_fitness * 100);
	}
}
