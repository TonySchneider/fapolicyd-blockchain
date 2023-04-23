#include <stdio.h>
#include <openssl/sha.h>

#define BUFSIZE 4096

char* sha256_file(char* path) {
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
    fclose(file);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);

    char *hex = malloc(sizeof(char) * (SHA256_DIGEST_LENGTH * 2 + 1));
    if (!hex) {
        fprintf(stderr, "Error allocating memory\n");
        return NULL;
    }

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(&hex[i * 2], "%02x", hash[i]);
    }

    hex[SHA256_DIGEST_LENGTH * 2] = '\0';
    return hex;
}

static char* hex_bytes_to_string(unsigned char* hex_bytes, size_t length) {
    char* hex_string = (char*) malloc(length * 2 + 1);
    if (hex_string == NULL) {
        return NULL;
    }
    for (size_t i = 0; i < length; i++) {
        sprintf(hex_string + 2 * i, "%02x", hex_bytes[i]);
    }
    hex_string[length * 2] = '\0';
    return hex_string;
}