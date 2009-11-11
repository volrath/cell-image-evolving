#include "evolve.h"

int file = 0;
Image original;
Image replica(Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), "white");
color_t px_original[IMAGE_WIDTH][IMAGE_HEIGHT];

bool compare(candidate_t *a, candidate_t *b) {
	return a->fitness > b->fitness;
}

poly_t clone(poly_t from) {
	poly_t copy;
	
	copy.color.r = from.color.r;
	copy.color.g = from.color.g;
	copy.color.b = from.color.b;
	
	for (int j = 0; j < NUM_VERT; j++) {
		copy.verts[j].x = from.verts[j].x;
		copy.verts[j].y = from.verts[j].y;
	}
	
	return copy;
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
		candidates_.push_back(new candidate_t());

	Color px;
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for(int j = 0; j < IMAGE_HEIGHT; j++) {
			px = original.pixelColor(i, j);
			px_original[i][j].from_pixel(px);
		}

	sort(candidates_.begin(), candidates_.end(), compare);
}

void population_t::next_generation() {
	sort(candidates_.begin(), candidates_.end(), compare);
	vector<candidate_t*> offspring;
	
	for (int i = 0; i < NUM_COOL_PARENTS; i++) {
		poly_t *dna;
		dna = candidates_[i]->dna;
		for (int j = 0; j < NUM_CHILDREN; j++) {
			int rnd_candidate = i;
			while (rnd_candidate == i)
				rnd_candidate = rand() % NUM_COOL_PARENTS;
			candidate_t *cand;
			cand = new candidate_t(dna, candidates_[rnd_candidate]->dna);
			offspring.push_back(cand);
		}
	}
	
	candidates_ = offspring;
}

candidate_t* population_t::get_fittest() {
	sort(candidates_.begin(), candidates_.end(), compare);
/*
 if (candidates_[0]->fitness > best)
		best = candidates_[0]->fitness;
	else
		it++;
	if (fact > .02)
		fact = .0;
	if (it > 5) {
		fact += .0025;
		it = 0;
		printf("  Mutate Chance: %f\n", MUTATE_CHANCE + fact);
	}
*/
	return candidates_[0];
}

/******************************** Candidate_t *********************************/
candidate_t::candidate_t() {
	dna = new poly_t[NUM_POLY];
	for (int i = 0; i < NUM_POLY; i++) {
		dna[i].color.r = rand() % 256;
		dna[i].color.g = rand() % 256;
		dna[i].color.b = rand() % 256;
		dna[i].color.alpha = rand() % 256;
		
		for (int j = 0; j < NUM_VERT; j++) {
			dna[i].verts[j].x = rand() % 128;
			dna[i].verts[j].y = rand() % 128;
		}
	}
	
	fitness = calc_fitness();
}

candidate_t::candidate_t(poly_t *parent1, poly_t *parent2) {
	dna = new poly_t[NUM_POLY];
	for (int i = 0; i < NUM_POLY; i++) {
		dna[i] = (RAND < .5) ? clone(parent1[i]) : clone(parent2[i]);
		
		int diff;
		if (RAND < MUTATE_CHANCE) {
			diff = dna[i].color.r + 256 * MUTATE_AMOUNT * (2 * RAND - 1.);
			if (diff > 255) dna[i].color.r = 255;
			else if (diff < 0) dna[i].color.r = 0;
			else dna[i].color.r = diff;
		}
		
		if (RAND < MUTATE_CHANCE) {
			diff = dna[i].color.g + 256 * MUTATE_AMOUNT * (2 * RAND - 1.);
			if (diff > 255) dna[i].color.g = 255;
			else if (diff < 0) dna[i].color.g = 0;
			else dna[i].color.g = diff;
		}
		
		if (RAND < MUTATE_CHANCE) {
			diff = dna[i].color.b + 256 * MUTATE_AMOUNT * (2 * RAND - 1.);
			if (diff > 255) dna[i].color.b = 255;
			else if (diff < 0) dna[i].color.b = 0;
			else dna[i].color.b = diff;
		}

		for (int j = 0; j < NUM_VERT; j++) {
			if (RAND < MUTATE_CHANCE) {
				diff = dna[i].verts[j].x + 128 * MUTATE_AMOUNT * (2 * RAND - 1.);
				if (diff > 127) dna[i].verts[j].x = 127;
				else if (diff < 0) dna[i].verts[j].x = 0;
				else dna[i].verts[j].x = diff;
			}

			if (RAND < MUTATE_CHANCE) {
				diff = dna[i].verts[j].y + 128 * MUTATE_AMOUNT * (2 * RAND - 1.);
				if (diff > 127) dna[i].verts[j].y = 127;
				else if (diff < 0) dna[i].verts[j].y = 0;
				else dna[i].verts[j].y = diff;
			}
		}
	}
	
	fitness = calc_fitness();
}

double candidate_t::calc_fitness() {
	int diff = 0;
	Color pr;
	draw();

	for (int i = 0; i < IMAGE_WIDTH; i++)
		for (int j = 0; j < IMAGE_HEIGHT; j++) {
			pr = replica.pixelColor(i, j);
			
			diff += abs((unsigned short int) px_original[i][j].r - ((unsigned short int) pr.redQuantum()   >> 8));
			diff += abs((unsigned short int) px_original[i][j].g - ((unsigned short int) pr.greenQuantum() >> 8));
			diff += abs((unsigned short int) px_original[i][j].b - ((unsigned short int) pr.blueQuantum()  >> 8));
		}

	return 1. - (double) diff / (double) (IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 255);
}

void candidate_t::draw() {
	list<Drawable> polygons;
	
	for (int i = 0; i < NUM_POLY; i++) {
		list<Coordinate> poly_coords;
		for (int j = 0; j < NUM_VERT; j++)
			poly_coords.push_back(Coordinate(dna[i].verts[j].x, dna[i].verts[j].y));

		polygons.push_back(DrawablePolygon(poly_coords));
		polygons.push_back(DrawableFillColor(Color(dna[i].color.r << 8, dna[i].color.g << 8, dna[i].color.b << 8, MaxRGB >> 2)));
	}
	
	replica.erase();
	replica.draw(polygons);
}

void candidate_t::write() {
	printf("    Dibujando... ");
	char *s;
	s = new char[30];
	sprintf(s, "replicas/image%d.jpg", file++);
	printf("%s\n", s);
	draw();
	replica.write(s);
}
