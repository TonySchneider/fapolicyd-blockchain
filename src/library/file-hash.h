#include <stdio.h>
#include <openssl/sha.h>

#define BUFSIZE 4096

char* sha256_file(char* path);
char* hex_bytes_to_string(unsigned char* hex_bytes, size_t length);