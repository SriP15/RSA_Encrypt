#include "numtheory.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <inttypes.h>
#include "randstate.h"
#include <stdlib.h>
#include <time.h>

//The Pseudocode for the following functions was given in the Assignment 5 document.

//Calculates gcd of a and b, storing the value in d.
//Returns nothing (void).
//
//d: an mpz_t variable to store the gcd result in. Must already be initialized.
//a: an mpz_t integer. Must already be initialized.
//b: an mpz_t integer. Must already be initialized.
void gcd(mpz_t d, mpz_t a, mpz_t b) {
    //Loop runs while b != 0.
    while (mpz_cmp_ui(b, 0) != 0) {
        //d = b
        mpz_set(d, b);
        //b = a (mod b)
        mpz_mod(b, a, b);
        //a = d
        mpz_set(a, d);
    }
    //d = a
    mpz_set(d, a);
}

//Calculates the modular inverse of a (mod n), storing the result in i.
//Returns nothing (void).
//
//i: an mpz_t variable that holds the value of the inverse. Must already be initialized.
//If no inverse is found, i will be 0.
//a: an mpz_t variable. Must already be initialized.
//n: an mpz_t variable. Must already be initialized.
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
    //Declaring mpz_t variables
    mpz_t r1;
    mpz_t r2;
    mpz_t t1;
    mpz_t t2;
    mpz_t temp1;
    mpz_t temp2;
    mpz_t q;

    //Initializing mpz_t variables to 0.
    mpz_inits(r1, r2, t1, t2, temp1, temp2, q, NULL);

    //t2 = 1
    mpz_set_ui(t2, 1);
    //r1 = n
    mpz_set(r1, n);
    //r2 = a
    mpz_set(r2, a);

    //Loop runs while r2 != 0.
    while (mpz_cmp_ui(r2, 0) != 0) {

        //q = r1/r2
        mpz_fdiv_q(q, r1, r2);
        //temp1 = r1
        mpz_set(temp1, r1);
        //temp2 = r2
        mpz_set(temp2, r2);
        //r1 = r2
        mpz_set(r1, r2);

        //temp2 = temp2 * q
        mpz_mul(temp2, temp2, q);
        //temp1 = temp1 * temp2
        mpz_sub(temp1, temp1, temp2);
        //r2 = temp1
        mpz_set(r2, temp1);

        //temp1 = t1; temp2 = t2
        mpz_set(temp1, t1);
        mpz_set(temp2, t2);

        //t1 = t2
        mpz_set(t1, t2);

        //temp2 = temp2 * q; temp1 = temp1 + temp2; t2 = temp1
        mpz_mul(temp2, temp2, q);
        mpz_sub(temp1, temp1, temp2);
        mpz_set(t2, temp1);
    }

    //i = t1
    mpz_set(i, t1);

    //if r1 > 1, execute below statement. else if t1 < 0, execute below statement.
    if (mpz_cmp_ui(r1, 1) > 0) {
        mpz_set_ui(i, 0);
    } else if (mpz_cmp_ui(t1, 0) < 0) {
        mpz_add(t1, t1, n);
        mpz_set(i, t1);
    }

    //clear mpz variables
    mpz_clears(r1, r2, t1, t2, temp1, temp2, q, NULL);
}

//Calculates the modular exponentiation of base ^ exponent (mod n).
//Returns nothing (void).
//
//out: mpz_t variable to store results in. Must already be initialized.
//base: mpz_t variable that is the base. Must already be initialized.
//exponent: mpz_t variable that is the exponent. Must already be initialized.
//modulus: mpz_t variable that is the modulus. Must already be initialized.
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    //Declaring and initializing mpz_t variables.
    mpz_t v, p, exp;
    mpz_inits(v, p, exp, NULL);

    //Setting v = 1.
    mpz_set_ui(v, 1);
    //Setting p = base.
    mpz_set(p, base);
    //Setting exp = exponent.
    mpz_set(exp, exponent);

    //Lopp runs while exp > 0.
    while (mpz_cmp_ui(exp, 0) > 0) {
        //If-statement executes if exp is odd
        if (mpz_odd_p(exp) != 0) {
            //v = v * p
            mpz_mul(v, v, p);
            //setting v = v (mod modulus)
            mpz_mod(v, v, modulus);
        }
        //p = p * p
        mpz_mul(p, p, p);
        //setting p = p (mod modulus)
        mpz_mod(p, p, modulus);
        //exp = exp/2
        mpz_fdiv_q_ui(exp, exp, 2);
    }
    //out = v
    mpz_set(out, v);
    //clear mpz_t variables
    mpz_clears(v, p, exp, NULL);
}

//Tests if a number is prime using the Miller-Rabin test.
//Returns true if the number is indicated as prime.
//Returns false if the number is indicated as composite.
//
//n: an mpz_t variable that is tested for primality. It must already be initialized.
//iters: a uint64_t that indicates the number of iterations that the Miller-rabin should be run.
bool is_prime(mpz_t n, uint64_t iters) {
    //Declaring and initializing mpz_t variables
    mpz_t s, r, j, y, roll, two, temp;
    mpz_inits(s, r, j, y, roll, temp, two, NULL);

    //r = n - 1
    mpz_sub_ui(r, n, 1);
    //two = 2
    mpz_set_ui(two, 2);
    //temp = n (mod 2)
    mpz_mod_ui(temp, n, 2);

    //The following two if-statements were inspired by the Miller-Rabin code Profesor Long
    //gave during the Cryptogrophy lecture

    // if n < 2 or temp = 0 and n != 2, clear mpz_t variables and return false.
    if ((mpz_cmp_ui(n, 2) < 0) || ((mpz_cmp_ui(temp, 0) == 0) && (mpz_cmp_ui(n, 2) != 0))) {
        mpz_clears(s, r, j, y, roll, temp, two, NULL);
        return false;
    }
    // if n = 2, temp or n = 3, clear mpz_t variables and return true.
    if ((mpz_cmp_ui(n, 3) == 0) || (mpz_cmp_ui(n, 2) == 0)) {
        mpz_clears(s, r, j, y, roll, temp, two, NULL);
        return true;
    }

    //Loop runs while r is even
    while (mpz_even_p(r) != 0) {
        //r = r / 2
        mpz_fdiv_q_ui(r, r, 2);
        //s = s + 1
        mpz_add_ui(s, s, 1);
    }

    //s = s - 1
    mpz_sub_ui(s, s, 1);

    for (uint64_t i = 1; i < iters; i += 1) {
        //Generating a random number from [2 to n-1]
        mpz_sub_ui(temp, n, 3);
        mpz_urandomm(roll, state, temp);
        mpz_add_ui(temp, temp, 1);
        mpz_mod(roll, roll, temp);
        mpz_add_ui(roll, roll, 2);

        //y = (roll^r) (mod n)
        pow_mod(y, roll, r, n);
        //temp = n - 1
        mpz_sub_ui(temp, n, 1);

        //If y != 1 and y != temp, execute if-statement
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, temp) != 0)) {
            mpz_set_ui(j, 1);
            //Loop runs while j <= s and y != temp
            while ((mpz_cmp(j, s) <= 0) && (mpz_cmp(y, temp) != 0)) {
                pow_mod(y, y, two, n);
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(s, r, j, y, roll, temp, two, NULL);
                    //The number is composite.
                    return false;
                }
                mpz_add_ui(j, j, 1);
            }
            if (mpz_cmp(y, temp) != 0) {
                mpz_clears(s, r, j, y, roll, temp, two, NULL);
                //The number is composite.
                return false;
            }
        }
    }
    //clearing mpz_t variables
    mpz_clears(s, r, j, y, roll, temp, two, NULL);
    //The number is prime.
    return true;
}
//Generates a new prime number which is stored in p.
//Returns nothing (void).
//
//p: an mpz_t variable that holds a prime number.
//bits: a uint64_t specifying the minimum number of bits that p should be.
//iters: a uint64_t specifying the number of iterations to run is_prime() with.
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    //Declaring and initializing mpz_t variables.
    mpz_t n, offset;
    mpz_inits(n, offset, NULL);

    bool test_prime = false;

    while (test_prime == false) {
        //Generating a random number from 0 to n^(bits) - 1
        mpz_urandomb(n, state, bits);
        //Adding an offset of 2^n bits to n
        mpz_ui_pow_ui(offset, 2, bits);
        mpz_add(n, n, offset);
        //Checking if new number is prime
        test_prime = is_prime(n, iters);
    }
    //Set p = n
    mpz_set(p, n);
    //clear mpz_t variables.
    mpz_clears(n, offset, NULL);
}
