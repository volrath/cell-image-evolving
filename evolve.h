#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <Magick++.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>

#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128

#define POP_SIZE 36 // Múltiplo de NUM_CHILDREN y múltiplo de 6 (SPEs)
#define NUM_POLY 48 // Múltiplo de 8 ((POLY_LENGTH * NUM_POLY) % 128 == 0)
#define NUM_VERT 6
#define POLY_LENGTH (4 + NUM_VERT * 2)
#define DNA_LENGTH  (NUM_POLY * POLY_LENGTH)

#define MUTATE_CHANCE .02
#define MUTATE_AMOUNT .1
#define NUM_CHILDREN  4
#define NUM_COOL_PARENTS (POP_SIZE / NUM_CHILDREN)
#define RGB_BITS (MaxRGB == 255 ? 0 : 8)
#define RAND ((double) (rand() % 10000 / 10000.))

using namespace std;
using namespace Magick;

struct color_t {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char alpha;

  void from_pixel(Color px) {
    r = (unsigned char) px.redQuantum();
    g = (unsigned char) px.greenQuantum();
    b = (unsigned char) px.blueQuantum();
    alpha = (unsigned char) px.alphaQuantum();
  }
};

class candidate_t {
public:
	float fitness;
	float dna[DNA_LENGTH];

	candidate_t();
	candidate_t(float*, float*);
	float calc_fitness();
	void draw();
	void write();
};

class population_t {
public:
	candidate_t* candidates[POP_SIZE];

	population_t(char*);
	void next_generation();
	candidate_t* get_fittest();
};
