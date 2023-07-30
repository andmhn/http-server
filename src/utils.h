#include <stdbool.h>
#include <stdlib.h>

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
