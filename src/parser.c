#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// checks if curr_line is get request
int is_get_req(char * curr_line){
	char get[4];
	get[3] = '\0';

	for(int i = 0; i < 3; i++){
		get[i] = curr_line[i];
	}
	return strcmp(get, "GET");
}

// returns get string
char* parse_get_req(char * req) {
	int len = strlen(req);
	char *curr_line = (char*)malloc(500 * sizeof(char));
	int pos = 0;

	while(pos <= len){
		memset(curr_line, 0, 500);
		int i = 0;
		while(req[pos] != '\n'){
			curr_line[i] = req[pos];
			pos++;
			i++;
		}
		pos += 2;
		curr_line[i+1] = '\0';
		
		if(is_get_req(curr_line) == 0) break;
	}

	return curr_line;
}

