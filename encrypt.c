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
    fprintf(stderr, "   Encrypts data using RSA encryption.\n");
    fprintf(stderr, "   Encrypted data is decrypted by the decrypt program.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   %s [OPTIONS]\n\n", val);
    fprintf(stderr, "OPTIONS\n"
                    "   -h              Display program help and usage.\n"
                    "   -v              Display verbose program output.\n"
                    "   -i infile       Input file of data to encrypt (default: stdin).\n"
                    "   -o outfile      Output file for decrypted data (default: stdout).\n"
                    "   -n pvfile       Public key file (default: rsa.pub).\n");
}

//Parses command-line options, and encrypts text from a given input file using a pbfile.
//Returns a 0 or 1 depending on succesful exit of program.
//
//argc: int that stores number of command-line options passed
//argv stores command-line options passed
int main(int argc, char **argv) {
    int64_t opt;

    //initializes verbose to false
    bool verbose = false;

    //opening files
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pbfile = fopen("rsa.pub", "r");

    mpz_t p, q, d, e, n, user, s;

    //creating a character array to hold user's name:
    //I was notified that this notation works to initialize the array from Awano on the cse13s discord.
    char username[256] = { 0 };

    mpz_inits(p, q, d, e, n, user, s, NULL);

    //Parsing command line options
    while ((opt = getopt(argc, argv, "i:o:n:vh")) != -1) {
        switch (opt) {
        case 'i':
            infile = fopen(optarg, "r");
            //if file can't be opened, print to standard error
            if (infile == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            //if file can't be opened, print to standard error
            if (outfile == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'n':
            pbfile = fopen(optarg, "r");
            //if file can't be opened, print to standard error
            if (pbfile == NULL) {
                fprintf(stderr, "%s: No such file or directory\n", optarg);
                return EXIT_FAILURE;
            }
            break;
        case 'v': verbose = true; break;
        case 'h':
            usage(argv[0]);
            return EXIT_FAILURE;
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    //read n, e, s, username values from pbfile.
    rsa_read_pub(n, e, s, username, pbfile);

    if (verbose) {
        printf("user = %s\n", username);
        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    mpz_set_str(user, username, 62);

    if (!rsa_verify(user, s, e, n)) {
        fprintf(stderr, "Error: invalid key.\n");
    }

    //close all files, clear mpz_t variables, and clear randstate
    rsa_encrypt_file(infile, outfile, n, e);
    mpz_clears(p, q, d, e, n, user, s, NULL);
    fclose(pbfile);
    fclose(infile);
    fclose(outfile);
}
