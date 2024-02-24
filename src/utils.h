#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define FILE_OK 0
#define FILE_NOT_EXIST 1
#define FILE_TOO_LARGE 2
#define FILE_READ_ERROR 3

char *read_file(const char *f_name, int *err, size_t *f_size);
bool is_dir(const char *path);
bool is_binary(const char *f_name);
bool ends_with(const char *str, const char *end_str);
bool has_permission(const char *filename);
bool str_starts_with(const char *str, const char *word);
int verify_filepath(const char *file_path);


// Logger
//=======

// set log_file filepr
void log_init(FILE * log_file);

// output to log_file with [MSG]:
void log_msg(const char* msg);

// output to log_file with [ERR]:
void log_perr(const char* err);
