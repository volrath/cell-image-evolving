#include "evolve.h"

using namespace std;
using namespace Magick;

Image original("media/mona-lisa-128x128.jpg");
color_t px_original[IMAGE_WIDTH][IMAGE_HEIGHT];
int file = 0;

bool compare(candidate_t *a, candidate_t *b) {
	return a->fitness > b->fitness;
}

/******************************** Population_t ********************************/
population_t::population_t() {
	srand(time(NULL));
	for (int i = 0; i < POP_SIZE; i++)
		candidates_.push_back(new candidate_t());

	// Initialize px_original
	Color px;
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for(int j = 0; j < IMAGE_HEIGHT; j++) {
			px = original.pixelColor(i, j);
			px_original[i][j].from_pixel(px);
		}
}

// TODO: MUTATE CHANCE!!!!!!!!
void population_t::next_generation() {
	sort(candidates_.begin(), candidates_.end(), compare);
	vector<candidate_t*> offspring;
	
	int rnd_candidate;
	int num_cool_parents = floor(get_size() * PARENT_CUT_OFF);
	int num_children = ceil(1 / PARENT_CUT_OFF);
	
	for (int i = 0; i < num_cool_parents; i++) {
		poly_t *dna;
		dna = candidates_[i]->dna;
		for (int j = 0; j < num_children; j++) {
			rnd_candidate = i;
			while (rnd_candidate == i)
				rnd_candidate = rand() % num_cool_parents;
			candidate_t *cand;
			cand = new candidate_t(dna, candidates_[rnd_candidate]->dna);
			offspring.push_back(cand);
		}
	}

	candidates_.clear();
	candidates_ = offspring;
	sort(candidates_.begin(), candidates_.end(), compare);
	candidates_.resize(POP_SIZE, new candidate_t());
}

int population_t::get_size() {
	return candidates_.size();
}

candidate_t* population_t::get_fittest() {
	sort(candidates_.begin(), candidates_.end(), compare);
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
		dna[i] = (rand() % 2) ? parent1[i] : parent2[i];
		
		if (!rand() % 50) { // Chance: 1/50
			(dna[i].color.r += (rand() - 50) % 100) %= 256;
			(dna[i].color.g += (rand() - 50) % 100) %= 256;
			(dna[i].color.b += (rand() - 50) % 100) %= 256;
			
			for (int j = 0; j < NUM_VERT; j++) {
				(dna[i].verts[j].x = (rand() - 25) % 50) %= 128;
				(dna[i].verts[j].y = (rand() - 25) % 50) %= 128;
			}
		}
	}
	
	fitness = calc_fitness();
}

double candidate_t::calc_fitness() {
	Color pr;
	int diff = 0;
	replica = new Image(Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), Color(0,0,0,0));
	draw();

	for (int i = 0; i < IMAGE_WIDTH; i++)
		for (int j = 0; j < IMAGE_HEIGHT; j++) {
			pr = (*replica).pixelColor(i, j);
			
			diff += abs((unsigned short int) px_original[i][j].r - (unsigned short int) pr.redQuantum()   / 256);
			diff += abs((unsigned short int) px_original[i][j].b - (unsigned short int) pr.greenQuantum() / 256);
			diff += abs((unsigned short int) px_original[i][j].g - (unsigned short int) pr.blueQuantum()  / 256);
		}
	
	return (double) diff / (double) (IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 256);
}

void candidate_t::draw() {
	list<Drawable> polygons;
	
	for (int i = 0; i < NUM_POLY; i++) {
		list<Coordinate> poly_coords;
		for (int j = 0; j < NUM_VERT; j++)
			poly_coords.push_back(Coordinate(dna[i].verts[j].x, dna[i].verts[j].y));

		polygons.push_back(DrawablePolygon(poly_coords));
		polygons.push_back(DrawableFillColor(Color(256*dna[i].color.r, 256*dna[i].color.g, 256*dna[i].color.b, MaxRGB/2)));
	}
	
	(*replica).size(Geometry(IMAGE_WIDTH, IMAGE_HEIGHT));
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for (int j = 0; j < IMAGE_HEIGHT; j++)
			(*replica).pixelColor(i, j, "white");
	
	(*replica).draw(polygons);
}

void candidate_t::write() {
	printf("    Dibujando... ");
	char *s;
	s = new char[30];
	sprintf(s, "media/replica%d.gif", file++);
	printf("%s\n", s);
	(*replica).write(s);
}
