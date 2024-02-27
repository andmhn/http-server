#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "src/parser.h"
#include "src/utils.h"

#define assert_str(a, b) assert(strcmp(a, b) == 0)

void test_parse_encoded_url(){
    assert_str(parse_encoded_url("HI%20I%20AM%20MOHAN"), "HI I AM MOHAN");
    assert_str(parse_encoded_url("%20HI%20I%20AM%20MOHAN"),
                     " HI I AM MOHAN");
    assert_str(parse_encoded_url("Ram%20%26%20%20Shyam"), "Ram &  Shyam");
    assert_str(parse_encoded_url("gnu%2Blinux"), "gnu+linux");
}

int main()
{
	// list all tests here
	test_parse_encoded_url();

	log_msg("All Tests Passed!");
	return 0;
}