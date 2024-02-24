#include "utils.h"
#include <criterion/criterion.h>
#include <stdio.h>
#include <stdlib.h>

Test(log_test , should_print_msg) 
{
	FILE *log_file = fopen("bin/server.log", "a");
	log_init(log_file);

	log_msg("Hello Info");
	log_perr("Hello Perr");
	fclose(log_file);
}
