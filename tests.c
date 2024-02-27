#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "src/parser.h"
#include "src/utils.h"

#define str_eq(a, b) assert(strcmp(a, b) == 0)

void test_parse_encoded_url(){
    str_eq(parse_encoded_url("HI%20I%20AM%20MOHAN"), "HI I AM MOHAN");
    str_eq(parse_encoded_url("Ram%20%26%2%20Shyam"), "Ram &  Shyam");
    str_eq(parse_encoded_url("%20HI%20I%20AM%20MOHAN")," HI I AM MOHAN");
    str_eq(parse_encoded_url("gnu%2Blinux"), "gnu+linux");
}

int main()
{
	// list all tests here
	test_parse_encoded_url();

	// if we got till here
	log_msg("All Tests Passed!");
	return 0;
}