#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <Magick++.h>
#include <iostream>
#include <vector>
#include <list>

#define IMAGE_WIDTH  128
#define IMAGE_HEIGHT 128
#define POP_SIZE 36
#define NUM_POLY 6
#define NUM_VERT 6
#define MUTATE_CHANCE 0.02
#define PARENT_CUT_OFF 0.25

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
  };

  void print(ostream &os) const {
    os << "[ " << (unsigned short int)r << ", " << (unsigned short int)g << ", " << (unsigned short int)b << ", " << (unsigned short int)alpha << " ]";
  };

};

inline ostream& operator<<(ostream &os, const color_t &s) {
	s.print(os);
	return(os);
};

struct vert_t {
  unsigned char x;
  unsigned char y;

  void print(ostream &os) const {
		os << "(" << (unsigned short int) x << "," << (unsigned short int) y << ") ";
	};

};

inline ostream& operator<<(ostream &os, const vert_t &s) {
	s.print(os);
	return(os);
}

struct poly_t {
  color_t color;
  vert_t verts[NUM_VERT];

  void print(ostream &os) const {
    os << color << " | ";
    for (int i = 0; i < NUM_VERT; i++)
      os << verts[i];
  };

};

inline ostream& operator<<(ostream &os, const poly_t &s) {
	s.print(os);
	return(os);
}

class candidate_t {
public:
	double fitness;
	poly_t *dna;
	Image *replica;

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
	candidate_t* get_fittest();
};
