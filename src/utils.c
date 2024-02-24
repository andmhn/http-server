#include "utils.h"
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

bool has_permission(const char *filename) {
    struct stat fb;
    int res = stat(filename, &fb);
    if (res == -1) {
        return false;
    }
    return true;
}

bool is_dir(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return false;
    }

    closedir(dir);
    return true;
}

bool ends_with(const char *str, const char *end_str) {
    size_t end_len = strlen(end_str);
    size_t pos = strlen(str) - end_len;

    for (size_t i = 0; i <= end_len; i++, pos++) {
        if (str[pos] != end_str[i]) {
            return false;
        }
    }
    return true;
}

bool is_binary(const char *f_name) {
    char *file_types[] = {"gif", "png", "jpg", "jpeg", "mp4", "webm", "ico"};

    for (int i = 0; i < 7; i++) {
        if (ends_with(f_name, file_types[i])) {
            return true;
        }
    }
    return false;
}

char *read_file(const char *f_name, int *err, size_t *f_size) {
    char *buffer;
    size_t length;
    FILE *f = fopen(f_name, "rb");
    size_t read_length;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);

        // 1 GiB; best not to load a whole large file in one string
        if (length > 1073741824) {
            *err = FILE_TOO_LARGE;

            fclose(f);
            return NULL;
        }

        buffer = (char *)malloc(length + 1);

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                free(buffer);
                *err = FILE_READ_ERROR;

                fclose(f);
                return NULL;
            }
        }

        fclose(f);

        *err = FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    } else {
        *err = FILE_NOT_EXIST;

        return NULL;
    }

    return buffer;
}

bool str_starts_with(const char *str, const char *word) {
    size_t word_len = strlen(word);
    for (size_t i = 0; i < word_len; i++) {
        if (word[i] != str[i])
            return false;
    }
    return true;
}

int verify_filepath(const char *file_path) {
    FILE *temp = fopen(file_path, "rb");
    if (temp == NULL) {
        return -1;
    }
    fclose(temp);
    return 0;
}

// Logger
//=======

static FILE * log_file;

// set log_file fileptr
void log_init(FILE * fp)
{
    log_file = fp;
}

// output to log_file 
void log_msg(char* msg)
{
    if(log_file == NULL)
        log_file = stdout;
    fprintf(log_file, "[MSG]: %s\n", msg);
}

// strerror to log_file
void log_perr(char* err)
{
    if(log_file == NULL)
        log_file = stdout;
    fprintf(log_file, "[ERR]: %s: %s\n", err, strerror(errno));
}
