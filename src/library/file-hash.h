#include <stdio.h>
#include <openssl/sha.h>

#define BUFSIZE 4096

int sha256_file(char* path, unsigned char* hash);
