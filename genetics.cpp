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
	

	for (int i = 0; i < POP_SIZE; i++) {
	  candidate_t *cand;
	  cand = new candidate_t();
	  candidates_.push_back(cand);
	}

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
		double *dna;
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
}

candidate_t* population_t::get_fittest() {
	sort(candidates_.begin(), candidates_.end(), compare);
	return candidates_[0];
}

/******************************** Candidate_t *********************************/
candidate_t::candidate_t() {
  dna = (double *)malloc(DNA_LENGTH * sizeof(double));
  double px, py;
  for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
    dna[i+0] = RAND;
    dna[i+1] = RAND;
    dna[i+2] = RAND;
    dna[i+3] = max(0.2, RAND * RAND);
    px = RAND; py = RAND;

    for(int j = 0; j < NUM_VERT; j++) {
      dna[i+4+j*2] = px + (RAND - 0.5); dna[i+4+j*2] = dna[i+4+j*2] < 0.0 ? dna[i+4+j*2] * -1 : dna[i+4+j*2];
      dna[i+5+j*2] = py + (RAND - 0.5); dna[i+5+j*2] = dna[i+5+j*2] < 0.0 ? dna[i+5+j*2] * -1 : dna[i+5+j*2];
    }
  }
  fitness = calc_fitness();
}

candidate_t::candidate_t(double *parent1, double *parent2) {
  dna = (double *)malloc(DNA_LENGTH * sizeof(double));
  double *parent, val;
  for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
    parent = (RAND < 0.5) ? parent1 : parent2;

    for (int j = 0; j < POLY_LENGTH; j++) {
      val = parent[i+j];
      if (RAND < MUTATE_CHANCE) {
	val += RAND * MUTATE_AMOUNT * 2 - MUTATE_AMOUNT;
	if (val < 0.0) val = 0.0;
	if (val > 1.0) val = 1.0;
      }
      dna[i+j] = val;
    }
  }
  fitness = calc_fitness();
}

double candidate_t::calc_fitness() {
	int diff = 0;
	draw();

	Color pr;
	for (int i = 0; i < IMAGE_WIDTH; i++)
		for (int j = 0; j < IMAGE_HEIGHT; j++) {
			pr = replica.pixelColor(i, j);
			
			diff += abs((unsigned short int) px_original[i][j].r - ((unsigned short int) pr.redQuantum()));
			diff += abs((unsigned short int) px_original[i][j].g - ((unsigned short int) pr.greenQuantum()));
			diff += abs((unsigned short int) px_original[i][j].b - ((unsigned short int) pr.blueQuantum()));
		}

	return 1. - (double) diff / (double) (IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 255);
}

void candidate_t::draw() {
  list<Drawable> polygons;
  
  for (int i = 0; i < DNA_LENGTH; i += POLY_LENGTH) {
    list<Coordinate> poly_coords;
    for (int j = 0; j < NUM_VERT; j++) {
      //cout << (int)round(dna[i+4+j*2] * IMAGE_WIDTH) << " " << (int)round(dna[i+5+j*2] * IMAGE_HEIGHT) << endl;
      poly_coords.push_back(Coordinate((int)round(dna[i+4+j*2] * IMAGE_WIDTH), (int)round(dna[i+5+j*2] * IMAGE_HEIGHT)));
    }

    Color c((int)round(dna[i] * MaxRGB), (int)round(dna[i+1] * MaxRGB), (int)round(dna[i+2] * MaxRGB));
    polygons.push_back(DrawableFillOpacity((double)dna[i+3]));
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
