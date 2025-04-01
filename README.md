# Assignment 5: Public Key Cryptography
In this program, we create an RSA encryption algorithm that can encrypt/decrypt any text file using generated public and private keys. It does this through the usage of three program files: keygen.c, encrypt.c, and decrypt.c. 
1. keygen.c generates the public and private keys. It writes the public and private keys to their respective files. 
2. encrypt.c encrypts text in a given input file (using the public key file generated from keygen), and outputs the encrypted data to some output file.
3. decrypt.c decrypts encrypted text files (using the private key file generated from keygen), and outputs the decrypted data to some output file.

The three files utilize functions from other files ---rsa.c, numtheory.c, randstate.c--- to help perform their functions. 

## How to build the program:
Before and after the program has been built, the created binary files can be removed with `$ make clean`. 

To compile the keygen program, enter `$ make keygen`. 
To compile the encrypt program, enter `$ make encrypt`. 
To compile the decrypt program, enter `$ make decrypt`. 

Entering `$ make all` or `$ make` can also build the three programs above.

## How to run the program:
To run the keygen program, enter `$ ./keygen (command-line options)`
To run the encrypt program, enter `$ ./encrypt (command-line options)`
To run the decrypt program, enter `$ ./decrypt (command-line options)`

## Command-line options:
The three files accept various command-line options as follows:

The options the keygen program accepts are the following:
- -b bits: specifies minimum bits needed for the public modulus n (default is 256)
- -i iterations: specifies iterations for testing prime numbers using Miller-Rabin (default is 50)
- -n pbfile: specifies the public key file (default is rsa.pub)
- -d pvfile: specifies the private key file (default is rsa.priv)
- -s seed: specifies the random seed for initializing random state (default is time(NULL))
- -v: specifies verbose output
- -h: display the usage message

The options the encrypt program accepts are the following:
- -i infile: specifies the input file for encryption (default is standard input)
- -o outfile: specifies the output file for encryption (default is standard output)
- -n pbfile: specifies the file with the public key (default is rsa.pub)
- -v: enables verbose output.
- -h: displays the usage message.

The options the decrypt program accepts are the following:
- -i infile: specifies the input file for decryption (default is standard input)
- -o outfile: specifies the output file for decryption (default is standard output)
- -n pvfile: specifies the file containing the private key (default: rsa.priv)
- -v: enables verbose output
- -h: displays the usage message

## Scan-build:
Scan-build revealed no errors when I ran it.

## Error Checking:
To detect input errors such as the file containing the RSA signature not being verifiable, files not opening, etc., I created certain if-statements that would print to standard error if an error was detected.
