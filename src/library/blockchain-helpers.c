#include <stdio.h>
#include <stdlib.h>

void read_trusted_file_names(const char* file_path, char** list, int list_size) {
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int i = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (i >= list_size) {
            fprintf(stderr, "List size too small\n");
            exit(EXIT_FAILURE);
        }
        // Remove newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';
        // Copy the line into the list
        list[i] = strdup(line);
        i++;
    }

    fclose(fp);
}