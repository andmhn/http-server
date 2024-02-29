#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "src/parser.h"
#include "src/utils.h"

#define str_eq(a, b) assert(strcmp(a, b) == 0)

void test_parse_encoded_url(){
    str_eq(parse_encoded_url("HI%20I%20AM%20MOHAN"), "HI I AM MOHAN");
    str_eq(parse_encoded_url("Ram%20%26%20%20Shyam"), "Ram &  Shyam");
    str_eq(parse_encoded_url("%20HI%20I%20AM%20MOHAN")," HI I AM MOHAN");
    str_eq(parse_encoded_url("gnu%2Blinux"), "gnu+linux");
    str_eq(
        parse_encoded_url("/yt/%E3%80%90%E6%9D%B1%E6%96%B9%E3%80%91Bad%20Apple!!%20%EF%BC%B0%EF%BC%B6%E3%80%90%E5%BD%B1%E7%B5%B5%E3%80%91.mp4"),
        "/yt/【東方】Bad Apple!! ＰＶ【影絵】.mp4"
    );
}

int main()
{
	// list all tests here
	test_parse_encoded_url();

	// if we got till here
	log_msg("All Tests Passed!");
	return 0;
}
