#include <stdio.h>
#include <stdlib.h>

#include "llist.h"

void load_trusted_file_names(const char* file_path, list_t* list) {
    char line[256], *file_name = NULL, *data = NULL;
    int escaped = 0;

    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), fp)) {
        // Remove newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';
        
        // define file name
        file_name = escaped ? unescape(line) : strdup(line);

        // Search for this file recursively in the system
        

        // Add a new item for the line and add it to the list
        if (list_append(list, file_name, data)) {
			free(file_name);
			free(data);
		}
    }

    fclose(fp);
}