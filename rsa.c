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

//Calculates the lcm of p and q.
//Returns nothing (void).
//
//p: an mpz_t variable. Must already be initialized.
//q: an mpz_t variable. Must already be initialized.
//out: an mpz_t variable that stores the lcm of p and q.
void lcm(mpz_t out, mpz_t p, mpz_t q) {
    //Initializing and declaring mpz_t variables
    mpz_t temp, temp2;
    mpz_inits(temp, temp2, NULL);
    //temp = p * q
    mpz_mul(temp, p, q);
    // temp = |temp|
    mpz_abs(temp, temp);
    gcd(temp2, p, q);
    //out = temp/temp2
    mpz_fdiv_q(out, temp, temp2);
    //clear mpz_t variables
    mpz_clears(temp, temp2, NULL);
}

//Creates an RSA public key, storing the values of p, q, n, and e.
//Returns nothing (void).
//
//p: an initialized mpz_t variable that will store the value of p.
//q: an initialized mpz_t variable that will store the value of q.
//n: an initialized mpz_t variable that will store the value of n (p * q).
//e: an initialized mpz_t variable that will store the value of the public exponent.
//nbits: a uint64_t that specifies minimum amount of bits that n should be.
//iters: a uint64_t that stores the number of is_prime() iterations.
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    //Initializing and delaring mpz_t variables
    mpz_t p2, q2, lcm_out, lcm_out_copy, e_copy, temp;
    mpz_inits(p2, q2, lcm_out, lcm_out_copy, e_copy, temp, NULL);

    uint64_t size;
    uint64_t pbits;
    uint64_t qbits;

    //calculating an pbits value in range [nbits/4,3*nbits/4], and qbits is nbits - pbits
    pbits = (random() % ((nbits / 2) + 1) + (nbits / 4));
    qbits = nbits - pbits;

    //Finding a prime number for n.
    do {

        make_prime(p, pbits, iters);
        make_prime(q, qbits, iters);

        mpz_mul(n, p, q);

        size = mpz_sizeinbase(n, 2);
    } while (size < nbits);

    mpz_sub_ui(p2, p, 1);
    mpz_sub_ui(q2, q, 1);

    lcm(lcm_out, p2, q2);
    mpz_set(lcm_out_copy, lcm_out);

    //Finding a public exponent e.
    do {
        mpz_urandomb(e, state, nbits);
        mpz_set(e_copy, e);
        gcd(temp, e_copy, lcm_out_copy);
        mpz_set(e_copy, e);
        mpz_set(lcm_out_copy, lcm_out);

    } while (mpz_cmp_ui(temp, 1) != 0);

    //clearing mpz_t variables
    mpz_clears(p2, q2, lcm_out, lcm_out_copy, e_copy, temp, NULL);
}

//Writes the values of n, e, s, and username to pbfile.
//Returns nothing (void).
//
//n: mpz_t value already calculated.
//e: mpz_t public exponent value already calculated.
//s: mpz_t signature value already calculated.
//username: string that is already determined.
//pbfile: Opened value to write to.
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

//Reads the values of n, e, s, and username from pbfile.
//Returns nothing (void).
//
//n: mpz_t that is read in from pbfile.
//e: mpz_t that is read in from pbfile.
//s: mpz_t that is read in from pbfile
//username: string read in my pbfile
//pbfile: Opened file to read.
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    gmp_fscanf(pbfile, "%s\n", username);
}

//Makes the corresponding private key d.
//Returns nothing (void).
//
//d: initialized mpz_t variable that will hold the value of private key.
//e: initialized mpz_t variable that holds the value of public exponent.
//p: initialized mpz_t that holds value of p.
//q: initialized mpz_t that holds value of q.
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {

    //Declaring and initializing mpz_t variables
    mpz_t p2, q2, lcm_out;
    mpz_inits(p2, q2, lcm_out, NULL);

    //setting p2 and q2 to 1 less than their respective p and q values
    mpz_sub_ui(p2, p, 1);
    mpz_sub_ui(q2, q, 1);

    //calculating lcm of p2 and q2
    lcm(lcm_out, p2, q2);

    mod_inverse(d, e, lcm_out);
    //Clearing mpz_t variables
    mpz_clears(p2, q2, lcm_out, NULL);
}

//Writes the values of n and d to pbfile.
//Returns nothing (void).
//
//n: mpz_t value already calculated.
//d: mpz_t private key value already calculated.
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

//Reads the values of n and d to pbfile.
//Returns nothing (void).
//
//n: mpz_t value (already initialized) to store n.
//d: mpz_t value (already initialized) to store d.
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}

//Encrypts a given value m, and stores it in c
//Returns nothing.
//
//c: mpz_t value of encrypted message.
//m: mpz_t value of given message to enrypt.
//e: mpz_t value of public exponent already set
//n: mpz_t value of n already set
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
}

//Encrypts a given text file in blocks.
//Returns nothing.
//
//infile: file to encrypt.
//outfile: file to output encrypted text to.
//n: mpz_t that has stored value of n.
//e: mpz_t that has stored value of e.
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    //setting k value for number of bytes in a block for encryption.
    uint64_t k = ((mpz_sizeinbase(n, 2)) - 1) / 8;

    //dynamically allocating memory for a block of text
    uint8_t *block;
    block = (uint8_t *) calloc(k, sizeof(uint8_t));

    size_t j;

    //Declaring and Initializing mpz_t variables
    mpz_t message, ciphertext;
    mpz_inits(message, ciphertext, NULL);
    //prepending block with a value
    block[0] = 0xFF;

    //Encrypting data in blocks
    do {
        for (uint32_t i = 1; i < k; i += 1) {
            block[i] = 0;
        }
        //reading text from file
        j = fread(block + 1, sizeof(uint8_t), k - 1, infile);

        if (j == 0) {
            break;
        }
        //converting the text to mpz_t value
        mpz_import(message, j + 1, 1, sizeof(uint8_t), 1, 0, block);
        //encrypting the block
        rsa_encrypt(ciphertext, message, e, n);
        //printing the encrypted block value to outfile
        gmp_fprintf(outfile, "%Zx\n", ciphertext);
    } while (j == (k - 1));

    //clearing mpz_ variables and freeing block memory
    mpz_clears(message, ciphertext, NULL);
    free(block);
}

//Decrypts a given ciphertext c, and stores it in m.
//Returns nothing.
//
//m: mpz_t variable with decrypted message stored.
//c: mpz_t variable with given ciphertext.
//d: mpz_t variable with private key already set.
//n: mpz_t variable with stored n value.
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
}

//Decrypts a given encrypted text file in blocks.
//Returns nothing.
//
//infile: encrypted file to decrypt.
//outfile: given file to print decrypted text to.
//n: mpz_t that has set value of n.
//mpz_t that has already set value of private key.
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    //setting k value for number of bytes in a block for encryption
    uint64_t k = ((mpz_sizeinbase(n, 2)) - 1) / 8;

    //dynamically allocating memory for a block of text
    uint8_t *block;
    block = (uint8_t *) calloc(k, sizeof(uint8_t));

    size_t j;

    //Declaring and Initializing mpz_t variables
    mpz_t message, ciphertext;
    mpz_inits(message, ciphertext, NULL);

    //Ensuring file pointer points to the first element in the file
    rewind(infile);

    //Reading text blocks from file while there are more of them, and decrypting them
    while (gmp_fscanf(infile, "%Zx\n", ciphertext) != EOF) {
        //decrypting ciphertext
        rsa_decrypt(message, ciphertext, d, n);
        //converting mpz_t variable into block value
        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, message);
        //writing decrypted text to outfile
        fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
    }
    //clearing mpz_t variables and freeing block
    mpz_clears(message, ciphertext, NULL);
    free(block);
}

//Signs RSA, by producing a signature
//Returns nothing.
//
//s: an mpz_t that stores the value of the signature
//m, d, and n are mpz_t variables that have already been set.
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n);
}

//Verifies RSA signature.
//Returns true if signature is verified, else returns false.
//m: an mpz_t that stores the actual value of the signature
//s, e, and n are mpz_t variables that have already been set.
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);

    pow_mod(t, s, e, n);

    if (mpz_cmp(m, t) == 0) {
        //signature is verified.
        mpz_clear(t);
        return true;
    } else {
        //signature could not be verified.
        mpz_clear(t);
        return false;
    }
}
