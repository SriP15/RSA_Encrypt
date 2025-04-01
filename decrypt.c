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
    fprintf(stderr, "   Decrypts data using RSA decryption.\n");
    fprintf(stderr, "   Encrypted data is encrypted by the encrypt program.\n\n");
    fprintf(stderr, "USAGE\n");
    fprintf(stderr, "   %s [OPTIONS]\n\n", val);
    fprintf(stderr, "OPTIONS\n"
                    "   -h              Display program help and usage.\n"
                    "   -v              Display verbose program output.\n"
                    "   -i infile       Input file of data to decrypt (default: stdin).\n"
                    "   -o outfile      Output file for decrypted data (default: stdout).\n"
                    "   -n pvfile       Private key file (default: rsa.priv).\n");
}

//Parses command-line options, reads the private key file, and prints decrypted text to outfile.
//Returns a 0 or 1 depending on succesful exit of program.
//
//argc: int that stores number of command-line options passed
//argv stores command-line options passed
int main(int argc, char **argv) {

    int64_t opt;

    //set verbose to false
    bool verbose = false;

    //open files
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *pvfile = fopen("rsa.priv", "r");

    mpz_t d, e, n, s;

    mpz_inits(d, e, n, s, NULL);

    //parse command-line options
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
            pvfile = fopen(optarg, "r");
            //if file can't be opened, print to standard error
            if (pvfile == NULL) {
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
    //read the private key file
    rsa_read_priv(n, d, pvfile);

    //verbose mode
    if (verbose) {
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }
    //decrypt the file
    rsa_decrypt_file(infile, outfile, n, d);

    //clear mz_t variables, and close files
    mpz_clears(d, e, n, s, NULL);
    fclose(pvfile);
    fclose(infile);
    fclose(outfile);
}
