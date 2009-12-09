#include "defines.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Magick++.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <pthread.h>
#include <sched.h>
#include <errno.h>
#include <altivec.h>
#include <libspe2.h>

#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128

using namespace std;
using namespace Magick;

typedef union {
  int i[4];
  vector int v;
} vect_u;

typedef struct ppu_pthread_data{
	spe_context_ptr_t context;
	pthread_t pthread;
	unsigned int entry;
	void *argp;
	void *envp;
	spe_stop_info_t stopinfo;
	pair_parents_t *parents;
}  ppu_pthread_data_t;

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
	float *dna;
	float fitness;

	candidate_t();
	candidate_t(pair_parents_t*, ppu_pthread_data_t*, int);
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


