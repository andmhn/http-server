#include "parser.h"
#include <criterion/criterion.h>

Test(parse_encoded_url, should_return_with_correct_character) {
    cr_assert_str_eq(parse_encoded_url("HI%20I%20AM%20MOHAN"), "HI I AM MOHAN");
    cr_assert_str_eq(parse_encoded_url("%20HI%20I%20AM%20MOHAN"),
                     " HI I AM MOHAN");
    cr_assert_str_eq(parse_encoded_url("Ram%20%26%20%20Shyam"), "Ram &  Shyam");
    cr_assert_str_eq(parse_encoded_url("work+life%20balance"),
                     "work life balance");
    cr_assert_str_eq(parse_encoded_url("gnu%2Blinux"), "gnu+linux");
}
