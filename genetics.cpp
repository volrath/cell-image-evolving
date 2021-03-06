#include "evolve.h"
#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>

int file = 0;
Image original;
Image replica(Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), "white");
color_t px_original[IMAGE_WIDTH][IMAGE_HEIGHT];

void *ppu_crossover(void *arg) {
  ppu_pthread_data_t *data = (ppu_pthread_data_t*) arg;
  spe_context_run(data->context, &(data->entry), 0, data->argp, data->envp, NULL);
  pthread_exit(NULL);
}

int compare_t(candidate_t *a, candidate_t *b) {
	return (a->fitness > b->fitness);
}

extern spe_program_handle_t spu_crossover_handle;

/******************************** Population_t ********************************/
population_t::population_t(char* file) {
	srand((unsigned) time(0));
	try {
		original.read(file);
	} catch (Exception &error) {
		fprintf(stderr, "%s\n", error.what());
		exit(1);
	}
	
	for (int i = 0; i < POP_SIZE; i++)
		candidates[i] = new candidate_t();

	Color px;
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for(int j = 0; j < IMAGE_HEIGHT; j++) {
			px = original.pixelColor(i, j);
			px_original[i][j].from_pixel(px);
		}
}

void population_t::next_generation() {
	std::sort(candidates, candidates + POP_SIZE, compare_t);
	candidate_t* offspring[POP_SIZE];
	ppu_pthread_data_t datap[NUM_CHILDREN];
	
	for (int i = 0; i < NUM_COOL_PARENTS; i++) {
		for (int j = 0; j < NUM_CHILDREN; j++) {
			int rnd_candidate = i;
			while (rnd_candidate == i) rnd_candidate = (int) floor(RAND * NUM_COOL_PARENTS);
			posix_memalign((void**) &datap[j].parents, (size_t) 128, sizeof(pair_parents_t));
			datap[j].parents->p1 = candidates[i]->dna;
			datap[j].parents->p2 = candidates[rnd_candidate]->dna;
			offspring[i * NUM_CHILDREN + j] = new candidate_t(datap[j].parents, &datap[j], j);
		}

		for (int j = 0; j < NUM_CHILDREN; j++) {
			pthread_join(datap[j].pthread, NULL);
			spe_context_destroy(datap[j].context);
		}

		for (int j = 0; j < NUM_CHILDREN; j++)
			offspring[i * NUM_CHILDREN + j]->calc_fitness();
	}
	
	for (int i = 0; i < POP_SIZE; i++)
		candidates[i] = offspring[i];
}

candidate_t* population_t::get_fittest() {
	std::sort(candidates, candidates + POP_SIZE, compare_t);
	return candidates[0];
}

/******************************** Candidate_t *********************************/
candidate_t::candidate_t() {
	posix_memalign((void**) &dna, (size_t) 128, sizeof(float)*DNA_LENGTH);
	for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
		dna[i+0] = RAND;
		dna[i+1] = RAND;
		dna[i+2] = RAND;
		dna[i+3] = max(.2, RAND * RAND);

		for(int j = 0; j < NUM_VERT; j++) {
			dna[i+4+j*2] = RAND;
			dna[i+5+j*2] = RAND;
		}
	}

	calc_fitness();
}

candidate_t::candidate_t(pair_parents_t* parents, ppu_pthread_data_t *datap, int x) {
	posix_memalign((void**) &dna, (size_t) 128, sizeof(float)*DNA_LENGTH);
	parents->sol = dna;

	datap->context = spe_context_create(0, NULL);
	spe_program_load(datap->context, &spu_crossover_handle);
	datap->entry = SPE_DEFAULT_ENTRY;
	datap->argp = (void*) parents;
	datap->envp = (void*) 128;
	pthread_create(&(datap->pthread), NULL, &ppu_crossover, datap);
	spe_in_mbox_write(datap->context, (unsigned int*) &x, 1, SPE_MBOX_ANY_NONBLOCKING);
}

float candidate_t::calc_fitness() {
	int diff = 0;
	draw();

	vect_u r, g, b;
	r.v = (vector int) {0, 0, 0, 0};
	g.v = (vector int) {0, 0, 0, 0};
	b.v = (vector int) {0, 0, 0, 0};
	Color pr1, pr2, pr3, pr4, pr;
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for (int j = 0; j < IMAGE_HEIGHT; j+=4) {
 			pr1 = replica.pixelColor(i, j);
 			pr2 = replica.pixelColor(i, j+1);
 			pr3 = replica.pixelColor(i, j+2);
 			pr4 = replica.pixelColor(i, j+3);

 			r.v = vec_add(r.v, (vector int) {
				abs((unsigned short int) px_original[i][j].r - ((unsigned short int) pr1.redQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+1].r - ((unsigned short int) pr2.redQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+2].r - ((unsigned short int) pr3.redQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+3].r - ((unsigned short int) pr4.redQuantum() >> RGB_BITS))
			});
 			g.v = vec_add(g.v, (vector int) {
				abs((unsigned short int) px_original[i][j].g - ((unsigned short int) pr1.greenQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+1].g - ((unsigned short int) pr2.greenQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+2].g - ((unsigned short int) pr3.greenQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+3].g - ((unsigned short int) pr4.greenQuantum() >> RGB_BITS))
			});
 			b.v = vec_add(b.v, (vector int) {
				abs((unsigned short int) px_original[i][j].b - ((unsigned short int) pr1.blueQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+1].b - ((unsigned short int) pr2.blueQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+2].b - ((unsigned short int) pr3.blueQuantum() >> RGB_BITS)),
				abs((unsigned short int) px_original[i][j+3].b - ((unsigned short int) pr4.blueQuantum() >> RGB_BITS))
			});
		}
	// Guarda el resultado en r
 	r.v = vec_add(r.v, vec_add(g.v, b.v));
 	diff = r.i[0] + r.i[1] + r.i[2] + r.i[4];

	return fitness = 1. - (float) diff / (float) (IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 255);
}

void candidate_t::draw() {
	list<Drawable> polygons;
	
	for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
		list<Coordinate> poly_coords;
		for (int j = 0; j < NUM_VERT; j++)
			poly_coords.push_back(Coordinate((int) round(dna[i+4+j*2] * IMAGE_WIDTH), (int) round(dna[i+5+j*2] * IMAGE_HEIGHT)));

		Color c((int) round(dna[i] * MaxRGB), (int) round(dna[i+1] * MaxRGB), (int) round(dna[i+2] * MaxRGB), (int) round(dna[i+3] * MaxRGB));
		polygons.push_back(DrawableFillColor(c));
		polygons.push_back(DrawablePolygon(poly_coords));
	}

	replica.erase();
	replica.draw(polygons);
}

void candidate_t::write() {
	char *s = new char[30];
	printf("    Dibujando... ");
	sprintf(s, "replicas/image%d.jpg", file++);
	printf("%s\n", s);
	draw();
	
	replica.write(s);
	delete [] s;
}

