#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "fapolicyd-backend.h"
#include "file-hash.h"
#include "llist.h"
#include "paths.h"

#define MAX_PATH_LENGTH 4096

const char *target_file_name;
const list_t* glob_list;
int target_file_size = 0;

static int validate_file(const char *file_path, const struct stat *sbuf, int type) {
    int current_file_path_size = strlen(file_path);
    unsigned long file_size;
    unsigned int tsource = SRC_BLOCKCHAIN;
    bool verified = false;
    char *data = NULL, *hash_value = NULL;

    if (type == FTW_F) {
        // TODO: We need change this condition since it takes file paths that ends with this file name but there are maybe characters before the name
        // For example: file name: test.py
        // Example path that it will get as well: testtest.py
        if(strcmp(&file_path[current_file_path_size-target_file_size], target_file_name) == 0){
            printf("Verifiying '%s'...\n", file_path);
            file_size = sbuf->st_size;
            
            // generate sha256 hash value
            hash_value = sha256_file(file_path);

            if (hash_value == NULL){
                free(target_file_name);
                return 1;
            }
            
            printf("Got hash value of %s: %s\n", (char *) file_path, hash_value);
            // Add a new item for the line and add it to the list
            if (verified){
                // define a data string format - "backend_number file_size hash_value"
                if (asprintf(&data, DATA_FORMAT, tsource, file_size, hash_value) == -1){
                    free(target_file_name);
                    return 1;
                }

                if (list_append(glob_list, file_path, data)) {
                free(target_file_name);
                free(data);
                }

                printf("The file '%s' is verified & added to the list\n", file_path);
            }
            else {
                printf("The file '%s' is not a trust file\n", file_path);
            }
        }
    }
    return 0;
}

static void append_verified_files(const char* file_name, list_t* list){
    target_file_name = file_name;
    target_file_size = strlen(file_name);

    printf("Handling '%s' trust file\n", (char *) file_name);
    // Search, generate hash, get verification & append the list
    ftw("/", validate_file, MAX_PATH_LENGTH);
}

/**
 * This function loads a file that contains trust file names.
 * It searches these files in the system, validates them 
 * and append to the list only the verfied files
*/
void load_trusted_file_names(list_t* list) {
    char line[256], *file_name = NULL, *data = NULL;
    int escaped = 0;

    FILE* fp = fopen(BLOCKCHAIN_TRUST_FILE_PATH, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    printf("Loading blockchain file names\n");
    while (fgets(line, sizeof(line), fp)) {
        // Remove newline character at the end of the line
        line[strcspn(line, "\n")] = '\0';
        
        // define file name
        file_name = escaped ? unescape(line) : strdup(line);

        // Search for this file recursively in the system
        append_verified_files(file_name, list);
    }

    fclose(fp);
}