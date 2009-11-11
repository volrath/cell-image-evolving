#include "evolve.h"

using namespace std;
using namespace Magick;

Image replica(Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), "white");
Image original("media/mona-lisa-128x128.jpg");
color_t px_original[IMAGE_WIDTH][IMAGE_HEIGHT];
int file = 0;

bool compare(candidate_t *a, candidate_t *b) {
	return a->fitness > b->fitness;
}

/******************************** Population_t ********************************/
population_t::population_t() {
  srand(time(NULL));
  
  // Initialize px_original
  Color px;
  for (int i = 0; i < IMAGE_WIDTH; i++) {
    for(int j = 0; j < IMAGE_HEIGHT; j++) {
      px = original.pixelColor(i, j);
      px_original[i][j].from_pixel(px);
    }
  }
  
  for (int i = 0; i < POP_SIZE; i++)
    candidates_.push_back(new candidate_t());
  sort(candidates_.begin(), candidates_.end(), compare);
}

void population_t::next_generation() {
	vector<candidate_t*> offspring;
	
	for (int i = 0; i < NUM_COOL_PARENTS; i++) {
		poly_t *dna;
		dna = candidates_[i]->dna;
		for (int j = 0; j < NUM_CHILDREN; j++) {
			int rnd_candidate = i;
			while (rnd_candidate == i)
			  rnd_candidate = floor(RAND * NUM_COOL_PARENTS);
			candidate_t *cand;
			cand = new candidate_t(dna, candidates_[rnd_candidate]->dna);
			offspring.push_back(cand);
		}
	}

	candidates_.clear();
	candidates_ = offspring;
//	for (int i = 0; i < (int) offspring.size(); i++)
//		candidates_.push_back(offspring[i]);
// 	for (int i = 0; i < 2 * NUM_CHILDREN; i++)
// 		candidates_.push_back(new candidate_t());
//	for (int i = 0; i < 10; i++) cout << 100*candidates_[i]->fitness << " "; cout << endl;
	candidates_.resize(POP_SIZE, new candidate_t());
	sort(candidates_.begin(), candidates_.end(), compare);
}

candidate_t* population_t::get_fittest() {
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
		dna[i] = (RAND < .5) ? parent1[i] : parent2[i];
		
		if (RAND < MUTATE_CHANCE)
			dna[i].color.r += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
		
		if (RAND < MUTATE_CHANCE)
			dna[i].color.g += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
		
		if (RAND < MUTATE_CHANCE)
			dna[i].color.b += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;

		if (RAND < MUTATE_CHANCE)
			dna[i].color.alpha += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;

		for (int j = 0; j < NUM_VERT; j++) {
	 		if (RAND < MUTATE_CHANCE) {
				dna[i].verts[j].x = RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
				if (dna[i].verts[j].x > 127) dna[i].verts[j].x = 127;
			}

			if (RAND < MUTATE_CHANCE) {
				dna[i].verts[j].y = RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
				if (dna[i].verts[j].y > 127) dna[i].verts[j].y = 127;
			}
		}
	}
	
	fitness = calc_fitness();
}

double candidate_t::calc_fitness() {
  Color pr;
  int diff = 0;
  draw();
  
  for (int i = 0; i < IMAGE_WIDTH; i++)
    for (int j = 0; j < IMAGE_HEIGHT; j++) {
      pr = replica.pixelColor(i, j);
      
      diff += abs((unsigned short int) px_original[i][j].r - (unsigned short int) pr.redQuantum());
      diff += abs((unsigned short int) px_original[i][j].b - (unsigned short int) pr.greenQuantum());
      diff += abs((unsigned short int) px_original[i][j].g - (unsigned short int) pr.blueQuantum());
    }
  
  return 1. - (double) diff / (double) (IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 256);
}

void candidate_t::draw() {
  list<Drawable> polygons;
  for (int i = 0; i < NUM_POLY; i++) {
    list<Coordinate> poly_coords;
    for (int j = 0; j < NUM_VERT; j++)
      poly_coords.push_back(Coordinate(dna[i].verts[j].x, dna[i].verts[j].y));

    Color c(dna[i].color.r, dna[i].color.g, dna[i].color.b, dna[i].color.alpha);
    polygons.push_back(DrawableFillColor(c));
    polygons.push_back(DrawableFillOpacity((double)(dna[i].color.alpha / 255.0)));
    polygons.push_back(DrawablePolygon(poly_coords));
  }
  replica.erase();
  replica.draw(polygons);
};

void candidate_t::write() {
	printf("    Dibujando... ");
	char *s;
	s = new char[30];
	sprintf(s, "media/replica%d.png", file++);
	printf("%s\n", s);
	draw();
	replica.write(s);
}
