#include <errno.h>
#include <stdio.h>
#include <string.h>

// Logger
//=======

static FILE * log_file;

// set log_file fileptr
void log_init(FILE * fp)
{
    log_file = fp;
}

// output to log_file 
void log_msg(const char* msg)
{
    if(log_file == NULL)
        log_file = stdout;
    fprintf(log_file, "%s\n", msg);
    fflush(log_file);
}

// strerror to log_file
void log_perr(const char* err)
{
    if(log_file == NULL)
        log_file = stderr;
    fprintf(log_file, "[ERR]: %s: %s\n", err, strerror(errno));
    fflush(log_file);
}
