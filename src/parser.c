#include "parser.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO add other requests
// checks if curr_line is get request
int find_method(char *curr_line) {
    char method[6];
    int i;
    for (i = 0; i < 4; i++) {
        method[i] = curr_line[i];
    }
    method[i + 1] = '\0';

    if (!strncmp(method, "GET ", 4)) { // space because length of method is 3
        return GET;
    }
    if (!strncmp(method, "HEAD", 4)) {
        return HEAD;
    }
    if (!strncmp(method, "POST", 4)) {
        return POST;
    }
    return -1;
}

// find values of request
void fill_req_content(const char *req_str, HttpRequest *request) {
    char *req_value = (char *)malloc(BUFSIZ * sizeof(char));

    // fix garbage returns
    memset(req_value, 0, BUFSIZ);

    int i = 0, pos = 0;
    while (req_str[pos] != ' ') // skipping method name
        pos++;

    pos++; // skip first space
    while (req_str[pos] != ' ') {
        req_value[i] = req_str[pos];
        pos++;
        i++;
    }
    req_value[pos] = '\0';
    request->value = req_value;
}

// returns 0 if valid request string
// returns -1 if invaild request
int parse_req(const char *req, HttpRequest *request) {
    size_t len = strlen(req);
    char curr_line[BUFSIZ];
    size_t pos = 0;

    // loop through every line in request
    while (pos <= len) {
        memset(curr_line, 0, BUFSIZ); // clear junk first

        // load current line
        int i = 0;
        while (req[pos] != '\n') {
            curr_line[i] = req[pos];
            pos++;
            i++;
        }
        pos++; // skip carriage character
        curr_line[i + 1] = '\0';

        // checking if curr_line is valid
        request->method = find_method(curr_line);
        if (request->method == -1)
            continue;
        else
            break;
    }
    if (request->method == -1) {
        log_msg("[ERR]: Got Invalid Request");
        return -1;
    }
    fill_req_content(curr_line, request);

    log_msg(curr_line);
    return 0;
}

#define MASK_UCHAR   ((unsigned char)~0)

// unsigned long to unsigned char
unsigned char ultouc(unsigned long ulnum)
{
  return (unsigned char)(ulnum & (unsigned long) MASK_UCHAR);
}

// parse Percent-encoding charcters in url
char *parse_encoded_url(const char *url) {
    char *parsed_url = (char*) malloc(sizeof(char) * BUFSIZ);
    memset(parsed_url, 0, BUFSIZ);

    char prev_char = url[0];
    char encoding[3]; // it will store encoded character after %

    int i = 0; // current position of url
    int j = 0; // position for parsed url
    for (; url[i] != '\0'; i++, j++) {
        if (url[i] == '%') {
            prev_char = url[i];
            j--;
            continue;
        }
        parsed_url[j] = url[i];

        if (prev_char == '%' && isxdigit(url[i]) && isxdigit(url[i + 1])) {
            // get encoded characters
            encoding[0] = url[i];
            encoding[1] = url[++i];
            encoding[2] = '\0';

            // Now convert the encoding hes str to hex value
            unsigned long hex = strtoul(encoding, NULL, 16);

            // change hex value to char equivalent
            parsed_url[j] = ultouc(hex);
        }
        prev_char = url[i];
    }
    parsed_url[j] = '\0'; // null termination
    return parsed_url;
}
