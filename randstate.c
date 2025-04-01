#include <stdint.h>
#include <gmp.h>
#include "randstate.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

//Initializing global variable state.
gmp_randstate_t state;

//Initializes a global variable state for gmp random functions
//using the Mersenne Twister algorithm.
//Function also seeds srandom().
//Returns nothing (void).
//
//seed: Accepts a uint64_t seed argument.
//Function should only be called once.
void randstate_init(uint64_t seed) {

    //Initializing state for Mersenne Twister algorithm.
    gmp_randinit_mt(state);

    //Seeding the initial value for state.
    gmp_randseed_ui(state, seed);

    //Seeding for calls to random().
    srandom(seed);
}

//Clears the created random state (state must already have been initialized).
//Returns nothing (void).
//
//Accepts no arguments (void).
void randstate_clear(void) {
    gmp_randclear(state);
}
