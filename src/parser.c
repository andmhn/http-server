#include "parser.h"
#include "utils.h"

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
        if (url[i] == '+') { // handle '+' first
            parsed_url[j] = ' ';
            continue;
        }
        parsed_url[j] = url[i];

        if (prev_char == '%') {
            // get encoded characters
            encoding[0] = url[i];
            encoding[1] = url[++i];
            encoding[2] = '\0';

            // assign encoding to appropriate character
            if (!strncmp(encoding, "20", 2))
                parsed_url[j] = ' ';
            else if (!strncmp(encoding, "26", 2))
                parsed_url[j] = '&';
            else if (!strncmp(encoding, "3A", 2))
                parsed_url[j] = ':';
            else if (!strncmp(encoding, "2F", 2))
                parsed_url[j] = '/';
            else if (!strncmp(encoding, "3F", 2))
                parsed_url[j] = '?';
            else if (!strncmp(encoding, "23", 2))
                parsed_url[j] = '#';
            else if (!strncmp(encoding, "5B", 2))
                parsed_url[j] = '[';
            else if (!strncmp(encoding, "5D", 2))
                parsed_url[j] = ']';
            else if (!strncmp(encoding, "40", 2))
                parsed_url[j] = '@';
            else if (!strncmp(encoding, "21", 2))
                parsed_url[j] = '!';
            else if (!strncmp(encoding, "24", 2))
                parsed_url[j] = '$';
            else if (!strncmp(encoding, "27", 2))
                parsed_url[j] = '\'';
            else if (!strncmp(encoding, "28", 2))
                parsed_url[j] = '(';
            else if (!strncmp(encoding, "29", 2))
                parsed_url[j] = ')';
            else if (!strncmp(encoding, "2A", 2))
                parsed_url[j] = '*';
            else if (!strncmp(encoding, "2B", 2))
                parsed_url[j] = '+';
            else if (!strncmp(encoding, "2C", 2))
                parsed_url[j] = ',';
            else if (!strncmp(encoding, "3B", 2))
                parsed_url[j] = ';';
            else if (!strncmp(encoding, "3D", 2))
                parsed_url[j] = '=';
            else if (!strncmp(encoding, "25", 2))
                parsed_url[j] = '%';
        }
        prev_char = url[i];
    }
    parsed_url[j] = '\0'; // null termination
    return parsed_url;
}
