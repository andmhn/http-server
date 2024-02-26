#include <errno.h>
#include <stdio.h>
#include <string.h>

// Logger
//=======

// output to log_file 
void log_msg(const char* msg)
{
    printf("%s\n", msg);
}

// strerror to log_file
void log_perr(const char* err)
{
    printf("[ERR]: %s : %s\n", err, strerror(errno));
}
