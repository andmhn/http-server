#include "parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char * get_file_str(char * req_str){
	char *req_file = (char*)malloc(BUFSIZ * sizeof(char));

	// fix garbage returns
	memset(req_file, 0,BUFSIZ);

	int i = 0, pos = 4;
	while(req_str[pos] != ' '){
		req_file[i] = req_str[pos];
		pos++;
		i++;
	}
	req_file[pos] = '\0';
	return req_file;	
}

// checks if curr_line is get request
int is_get_req(char * curr_line){
	char get[4];
	get[3] = '\0';

	for(int i = 0; i < 3; i++){
		get[i] = curr_line[i];
	}
	return strcmp(get, "GET");
}

// returns get request string
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
