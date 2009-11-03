//#include "genetics.cpp"
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <Magick++.h>

#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128

#define POP_SIZE 42
#define NUM_POLY 128
#define NUM_VERT 6
#define MUTATE_CHANCE 0.02

#define PARENT_CUT_OFF 0.25

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

Magick::Image original("media/mona-lisa-128x128.jpg");

class candidate_t {
 public:
  poly_t dna[NUM_POLY];
  int fitness;
  Magick::Image replica; //(Magick::Geometry(IMAGE_WIDTH, IMAGE_HEIGHT), Magick::Color(0,0,0,0));

  candidate_t();
  candidate_t(poly_t, poly_t);
  bool operator<(const candidate_t) const;

  int calc_fitness();
  void draw();
};

class population_t {
public:
  population_t();
  void next_generation();
  int get_size();
  candidate_t get_fittest();
};

