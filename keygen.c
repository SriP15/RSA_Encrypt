#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include <stdlib.h>
#include "numtheory.h"
#include <inttypes.h>
#include "rsa.h"
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

//Pseudocode for this file is given in the Assignment 5 doc.

//Prints the usage message and synopsis to standard error.
//Returns nothing.
//
//val: A string denoting the name of the file when called.
void usage(char *val) {
    fprintf(stderr, "SYNOPSIS\n");
    fprintf(stderr, "   Generates an RSA public/private key pair.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   %s [OPTIONS]\n\n", val);
    fprintf(stderr, "OPTIONS\n"
                    "   -h              Display program help and usage.\n"
                    "   -v              Display verbose program output.\n"
                    "   -b bits         Minimum bits needed for public key n (default: 256).\n"
                    "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n"
                    "   -n pbfile       Public key file (default: rsa.pub).\n"
                    "   -d pvfile       Private key file (default: rsa.priv).\n"
                    "   -s seed         Random seed for testing.\n");
}

//Parses command-line options, and writes public and private keys to their respective file.
//Returns a 0 or 1 depending on succesful exit of program.
//
//argc: int that stores number of command-line options passed
//argv stores command-line options passed
int main(int argc, char **argv) {
    uint64_t bits = 256, iters = 50, seed, pb_fd, pv_fd;
    int64_t opt;

    //setting default verbose value
    bool verbose = 0;

    //opening key files
    FILE *pbfile = fopen("rsa.pub", "w");
    FILE *pvfile = fopen("rsa.priv", "w");

    //Declaring and initializing mpz_t variables
    mpz_t p, q, d, e, n, username, s;
    mpz_inits(p, q, d, e, n, username, s, NULL);

    //setting default values
    iters = 50;
    seed = time(NULL);

    //Parsing command line options
    while ((opt = getopt(argc, argv, "b:i:n:d:s:vh")) != -1) {
        switch (opt) {
        case 'b': bits = (uint64_t) strtoull(optarg, NULL, 10); break;
        case 'i': iters = (uint64_t) strtoull(optarg, NULL, 10); break;
        case 'n':
            pbfile = fopen(optarg, "w");
            //if file can't be opened, print to standard error
            if (pbfile == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'd':
            pvfile = fopen(optarg, "w");
            //if file can't be opened, print to standard error.
            if (pvfile == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 's': seed = (uint64_t) strtoull(optarg, NULL, 10); break;
        case 'v': verbose = true; break;
        case 'h':
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    //setting file permissions to 0600
    pb_fd = fileno(pbfile);
    pv_fd = fileno(pvfile);

    fchmod(pb_fd, S_IRUSR | S_IWUSR);
    fchmod(pv_fd, S_IRUSR | S_IWUSR);

    //setting random state
    randstate_init(seed);

    //create public and private keys
    rsa_make_pub(p, q, n, e, bits, iters);
    rsa_make_priv(d, e, p, q);

    //get username
    char *user = getenv("USER");
    mpz_set_str(username, user, 62);

    rsa_sign(s, username, d, n);

    //write to public and private key files
    rsa_write_pub(n, e, s, user, pbfile);
    rsa_write_priv(n, d, pvfile);

    //verbose mode
    if (verbose) {
        printf("user = %s\n", user);
        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%zu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%zu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    //close all files, clear mpz_t variables, and clear randstate
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, d, e, n, username, s, NULL);
}
