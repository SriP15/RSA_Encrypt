CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)

all: keygen encrypt decrypt

keygen: keygen.o numtheory.o randstate.o rsa.o
	$(CC) -o keygen keygen.o numtheory.o randstate.o rsa.o $(LFLAGS)

encrypt: encrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o encrypt encrypt.o numtheory.o randstate.o rsa.o $(LFLAGS)

decrypt: decrypt.o numtheory.o randstate.o rsa.o
	$(CC) -o decrypt decrypt.o numtheory.o randstate.o rsa.o $(LFLAGS)

randstate.o: randstate.c randstate.h
	$(CC) $(CFLAGS) -c randstate.c

numtheory.o: numtheory.c numtheory.h randstate.h
	$(CC) $(CFLAGS) -c numtheory.c

rsa.o: rsa.c rsa.h numtheory.h randstate.h
	$(CC) $(CFLAGS) -c rsa.c

keygen.o: keygen.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c keygen.c

encrypt.o: encrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c encrypt.c

decrypt.o: decrypt.c numtheory.h randstate.h rsa.h
	$(CC) $(CFLAGS) -c decrypt.c

clean:
	rm -f keygen *.o
	rm -f encrypt *.o
	rm -f decrypt *.o

format:
	clang-format -i -style=file *.h
	clang-format -i -style=file *.c
