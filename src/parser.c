#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// checks if curr_line is get request
int find_method(char *curr_line) {
    char method[6];
    int i;
    for (i = 0; i < 4; i++) {
        method[i] = curr_line[i];
    }
    method[i + 1] = '\0';

    if (!strcmp(method, "GET ")) { // space because length of method is 3
        return GET;
    }
    if (!strcmp(method, "HEAD")) {
        return HEAD;
    }
    if (!strcmp(method, "POST")) {
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
    int len = strlen(req);
    char *curr_line = (char *)malloc(BUFSIZ * sizeof(char)); // current line
    int pos = 0;

    fflush(stdout); // TODO: fix this

    // loop through every line in request
    while (pos <= len) {
        memset(curr_line, 0, 500); // clear junk first

        int i = 0;
        while (req[pos] != '\n') {
            curr_line[i] = req[pos];
            pos++;
            i++;
        }
        curr_line[i + 1] = '\0';
        // printf(" curr line: %s\n", curr_line);
        // checking if curr_line is valid
        request->method = find_method(curr_line);
        if (request->method == -1)
            continue;
        else
            break;
        pos += 2;
    }
    if (request->method == -1) {
        fputs("Invalid Request\n", stderr);
        return -1;
    }
    fill_req_content(curr_line, request);

    printf("%s\n", curr_line);
    free(curr_line);
    return 0;
}
