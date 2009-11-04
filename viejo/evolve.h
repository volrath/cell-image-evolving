#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <Magick++.h>
#include <iostream>
#include <vector>
#include <list>

#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128
#define POP_SIZE 42
#define NUM_POLY 32
#define NUM_VERT 6
#define MUTATE_CHANCE 0.02
#define PARENT_CUT_OFF 0.25

using namespace std;
using namespace Magick;

Image original("media/mona-lisa-128x128.jpg");

struct color_t {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char alpha;
};

struct verts_t {
	unsigned char x;
	unsigned char y;
};

struct poly_t {
	color_t color;
	verts_t verts[6];
};

class candidate_t {
public:
	double fitness;
	poly_t *dna;
	Image replica;
	
	candidate_t();
	candidate_t(poly_t*, poly_t*);
	double calc_fitness();
	void draw();
	void write();
};

class population_t {
public:
	vector<candidate_t*> candidates_;
	population_t();
	void next_generation();
	int get_size();
	candidate_t* get_fittest();
};

