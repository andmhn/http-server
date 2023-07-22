#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init(void);

char SERVING_DIR[BUFSIZ];

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("Usage: http-server <folder to serve>");
		exit(1);
	}
	strncpy(SERVING_DIR, argv[1], strlen(argv[1]));

	printf("Servin in: %s\n\n", SERVING_DIR);

	init();	
}
