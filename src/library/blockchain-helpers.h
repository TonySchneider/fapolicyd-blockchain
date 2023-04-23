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

static int validate_file(const char *file_path, const struct stat *sbuf, int type);
static void append_verified_files(const char* file_name, list_t* list);
void load_trusted_file_names(const char* file_path, list_t* list);