#include <time.h>
#include <list>
#include <math.h>

#include "lib.h"


//vector<candidate_t> candidates_;

population_t::population_t() {
  srand(time(NULL));
  for (int i = 0; i < POP_SIZE; i++)
    candidates_.push_back(new candidate_t());
};

void population_t::next_generation() { // MUTATE CHANCE!!!!!!!!
    // Sort the candidates
    sort(candidates_.begin(), candidates_.end());
    vector<candidate_t> offspring;

    int num_cool_parents = floor(POP_SIZE * PARENT_CUT_OFF);
    int num_children = ceil(1 / PARENT_CUT_OFF);
    int rnd_candidate;

    for (int i = 0; i < num_cool_parents; i++) {
      poly_t dna = candidates[i].dna;
      for (int j = 0; j < num_children; j++) {
	rnd_candidate = i;
	while(rnd_candidate == i)
	  rnd_candidate = rand() % num_cool_parents;
	offspring.push_back(new candidate_t(dna, candidates[rnd_candidate].dna));
      }
    }

    candidates = offspring;
    candidates.resize(POP_SIZE, NULL);
  };

int population_t::get_size() {
    return candidates.size();
  };

candidate_t population_t::get_fittest() {
    sort(candidates_.begin(), candidates_.end());
    return candidates[0];
  };

class candidate_t {
  candidate_t() {
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

    replica.blank_image(Magick::Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), Magick::Color(0,0,0,0));
    fitness = calc_fitness();
  };

  candidate_t(poly_t *parent1, poly_t *parent2) {
    for  (int i = 0; i < NUM_POLY; i++) {
      dna[i] = (rand() % 2) ? parent1[i] : parent2[i];
    }

    fitness = calc_fitness();
  };

  bool operator<(const candidate_t o) const { return this.fitness < o.fitness };

  int calc_fitness() {
    Magick::Color pr, po;
    draw();

    int diff;
    for (i = 0; i < IMAGE_WIDTH; i++) {
      for(j = 0; j < IMAGE_HEIGHT; j++) {
	pr = replica.pixelColor(i, j);
	po = original.pixelColor(i, j);

	diff += abs((unsigned short int)pr.redQuantum() - (unsigned short int)po.redQuantum());
	diff += abs((unsigned short int)pr.blueQuantum() - (unsigned short int)po.blueQuantum());
	diff += abs((unsigned short int)pr.greenQuantum() - (unsigned short int)po.greenQuantum());
      }
    }

    return (1 - diff / IMAGE_HEIGHT * IMAGE_WIDTH * 3 * 256);
  };

  void draw() {
    list<Magick::Drawable> polygons;

    for(int i = 0; i < NUM_POLY; i++) {
      list<Magick::Coordinate> poly_coords;
      for (int j = 0; j < NUM_VERT; j++) {
	poly_coords.push_back(Magick::Coordinate(dna[i].verts[j].x, dna[i].verts[j].y));
      }
      polygons.push_back(Magick::DrawablePolygon(poly_coords));
    }

    replica.draw(polygons);
    replica.display();
  }
};

