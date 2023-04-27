#include <ftw.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <jansson.h>

#include "fapolicyd-backend.h"
#include "file-hash.h"
#include "llist.h"
#include "paths.h"

#define MAX_PATH_LENGTH 4096

const char *target_file_name;
const list_t* glob_list;
int target_file_length = 0;

int get_is_verified(const char* json_string) {
    json_error_t error;
    json_t* root = json_loads(json_string, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON string: %s\n", error.text);
        return -1;
    }
    json_t* is_verified = json_object_get(root, "isVerified");
    if (!json_is_string(is_verified)) {
        fprintf(stderr, "Error retrieving 'isVerified' key\n");
        json_decref(root);
        return -1;
    }
    const char* value = json_string_value(is_verified);
    int result = strcmp(value, "true") == 0 ? 1 : 0;
    json_decref(root);
    return result;
}

size_t static curl_write(void *buffer, size_t size, size_t nmemb, void *userp)
{
     userp += strlen(userp);  // Skipping to first unpopulated char
     memcpy(userp, buffer, nmemb);  // Populating it.
     return nmemb;
}

// function to send an API request and check if "isVerified" is true in the response
int is_verified(char* file_path, char* hash_value) {
    CURL *curl;
    CURLcode res;
    char url[1000], response[1000] = {0};
    int is_verified = 0;

    // create the URL for the API request
    sprintf(url, "http://vonage1.eastus.cloudapp.azure.com:9000/verify_credential_app/%s/%s", file_path, hash_value);

    // initialize curl
    curl = curl_easy_init();

    if(curl) {
	// set the URL for the API request
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // send the API request and retrieve the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

        // printf("response - '%s'\n", response);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 0;
        }

        // printf("Response content - '%s'", response);

        // cleanup curl
        curl_easy_cleanup(curl);
    }
    is_verified = get_is_verified(response);

    return is_verified;
}

static int validate_file(const char *file_path, const struct stat *sbuf, int type) {
    int current_file_path_size = strlen(file_path), verification_result;
    unsigned long file_size;
    unsigned int tsource = SRC_BLOCKCHAIN;
    char *data = NULL, *hash_value = NULL, *filename_with_delim, *index = NULL;
    const char *delim = "/";

    filename_with_delim = malloc(strlen(target_file_name) + strlen(delim) + 1);
    sprintf(filename_with_delim, "%s%s", delim, target_file_name);

    if (type == FTW_F) {
        if(strcmp(&file_path[current_file_path_size - strlen(filename_with_delim)], filename_with_delim) == 0){
            free(filename_with_delim);
            
            printf("Verifiying '%s'...\n", file_path);
            file_size = sbuf->st_size;
            
            // generate sha256 hash value
            hash_value = sha256_file(file_path);

            if (hash_value == NULL){
                free(target_file_name);
                return 1;
            }
                        
            // TODO: send api request to verify the file 
            printf("Sending API request with hash of %s: %s\n", (char *) file_path, hash_value);
            verification_result = is_verified(target_file_name, hash_value);
            
            // Add a new item for the line and add it to the list
            if (verification_result == 1){
                // printf("The file '%s' is verified\n", file_path);

                // define a data string format - "backend_number file_size hash_value"
                if (asprintf(&data, DATA_FORMAT, tsource, file_size, hash_value) == -1){
                    return 1;
                }

                free(hash_value);

                // printf("before the list_append\n");
                index = strdup(file_path);

                if (list_append(glob_list, index, data)) {
                    free(index);
                    free(data);
                }

                printf("The file '%s' verifeid & added to the trust list\n", file_path);
            }
            else {
                printf("The file '%s' is not a trust file\n", file_path);
            }
        }
    }

    return 0;
}

static void append_verified_files(const char* file_name, list_t* list){
    glob_list = list;
    target_file_name = file_name;
    target_file_length = strlen(file_name);

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

        free(file_name);
    }

    fclose(fp);
}