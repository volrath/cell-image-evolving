#include "evolve.h"

int file = 0;
Image original;
Image replica(Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), "white");
color_t px_original[IMAGE_WIDTH][IMAGE_HEIGHT];

extern spe_program_handle_t spu_crossover;

bool compare(candidate_t *a, candidate_t *b) {
	return a->fitness > b->fitness;
}

void *ppu_crossover(void *args) {
  ppu_pthread_data_t *data = (ppu_pthread_data_t *) arg;
  spe_context_run(data->context,&data->entry,0,data->argp,data->envp,NULL);
  pthread_exit(NULL);
}

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

	sort(candidates, candidates + POP_SIZE, compare);
}

void population_t::next_generation() {
	sort(candidates, candidates + POP_SIZE, compare);
	candidate_t* offspring[POP_SIZE];
	ppu_pthread_data_t datap[NUM_CHILDREN];
	
	for (int i = 0; i < NUM_COOL_PARENTS; i++) {
		for (int j = 0; j < NUM_CHILDREN; j++) {
			int rnd_candidate = i;
			while (rnd_candidate == i)
				rnd_candidate = floor(RAND * NUM_COOL_PARENTS);
			offspring[i * NUM_CHILDREN + j] = new candidate_t(candidates[i]->dna, candidates[rnd_candidate]->dna, &datap[j]);
		}
		for (int j = 0; j < NUM_CHILDREN; j++) {
		  pthread_join(datap[j].pthread,NULL);
		  spe_context_destroy(datap[j].context);
		}
	}
	
	for (int i = 0; i < POP_SIZE; i++)
		candidates[i] = offspring[i];
}

candidate_t* population_t::get_fittest() {
	sort(candidates, candidates + POP_SIZE, compare);
	return candidates[0];
}

/******************************** Candidate_t *********************************/
candidate_t::candidate_t() {
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

	fitness = calc_fitness();
}

candidate_t::candidate_t(float *parent1, float *parent2, ppu_pthread_data_t *datap) {
  datap->context = spe_context_create(0,NULL);
  spe_program_load(datap[i].context,&spu_crossover);
  datap->entry = SPE_DEFAULT_ENTRY;
  datap->argp = (void *) buffer;
  datap->flags = 0;
  datap->envp = (void *) 128;
  pthread_create(&pthread,NULL,&ppu_pthread_function,&datap);

  float *parent, val;
	for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
		parent = (RAND < 0.5) ? parent1 : parent2;

		for (int j = 0; j < POLY_LENGTH; j++) {
			val = parent[i+j];
			if (RAND < MUTATE_CHANCE) {
				val += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
				if (val < 0.) val = 0.;
				if (val > 1.) val = 1.;
			}
			dna[i+j] = val;
		}
	}

	fitness = calc_fitness();
}

float candidate_t::calc_fitness() {
	int diff = 0;
	draw();

	Color pr;
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for (int j = 0; j < IMAGE_HEIGHT; j++) {
			pr = replica.pixelColor(i, j);
			
			diff += abs((unsigned short int) px_original[i][j].r - ((unsigned short int) pr.redQuantum()   >> RGB_BITS));
			diff += abs((unsigned short int) px_original[i][j].g - ((unsigned short int) pr.greenQuantum() >> RGB_BITS));
			diff += abs((unsigned short int) px_original[i][j].b - ((unsigned short int) pr.blueQuantum()  >> RGB_BITS));
		}

	return 1. - (float) diff / (float) (IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 255);
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
