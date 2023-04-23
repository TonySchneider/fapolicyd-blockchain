#include <stdio.h>
#include <openssl/sha.h>

#define BUFSIZE 4096

int sha256_file(char* path, unsigned char* hash) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return 0;
    }

    SHA256_CTX ctx;
    SHA256_Init(&ctx);

    unsigned char buf[BUFSIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buf, 1, BUFSIZE, file))) {
        SHA256_Update(&ctx, buf, bytes_read);
    }

    SHA256_Final(hash, &ctx);

    fclose(file);
    return 1;
}
