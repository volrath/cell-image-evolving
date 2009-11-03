#include <stdio.h>

#include "lib.h"

//using namespace std;

int main() {
  population_t pop();// pop = new population_t();
  int gen_count = 0;

  while(1) {
    pop->next_generation();
    candidate_t fittest = pop->get_fittest();

    if (!(gen_count++ % 50))
      fittest.draw();
  }
};
