#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/socket.h>

extern char SERVING_DIR[];

struct entry {
    char *data;
    LIST_ENTRY(entry) entries; /* List */
};

LIST_HEAD(listhead, entry);

char *make_header(const char *folder) {
    char *header = malloc(BUFSIZ);

    char *format = "<!DOCTYPE html>\
<head>\
	<title>%s</title>\
</head>\
\
";

    sprintf(header, format, folder);

    return header;
}

char* strip_root_folder(const char* path){
	char * stripped = malloc(strlen(path) * sizeof(char));
	int pos = (int)strlen(SERVING_DIR);
	int i = 0;
	while(path[pos] != '\0')
		stripped[i++] = path[pos++];

	stripped[i] = '\0';
	return stripped;
}

// constructs html
char *make_html_list(const char *content, const char *folder_name) {
    char *list = malloc(BUFSIZ * sizeof(char));
	memset(list, 0, BUFSIZ);

	char * root = strip_root_folder(folder_name);
    sprintf(list, "<li><a href=\"%s/%s\">%s</a></li>", root, content, content);

	free(root);
    return list;
}

void get_folder_contents(const char *folder_name,
                         struct listhead *content_list) {
    struct dirent *entry;
    DIR *fb = opendir(folder_name);

    if (fb == NULL) {
        perror("opendir");
        return;
    }

	// TODO skip . and ..
    while ((entry = readdir(fb))) {

        struct entry *content_node = malloc(sizeof(struct entry));
        content_node->data = malloc(BUFSIZ * sizeof(char));
		memset(content_node->data, 0, BUFSIZ);
        strncat(content_node->data, entry->d_name, strlen(entry->d_name));

        LIST_INSERT_HEAD(content_list, content_node, entries);
    }
    closedir(fb);
}

void send_folder_content(const char *folder_name, int client_fd) {
	// TODO check if exist index.html file
    struct listhead content_list;
    LIST_INIT(&content_list);

	puts(folder_name);
    get_folder_contents(folder_name, &content_list);

    char *header = make_header(folder_name);
    send(client_fd, header, strlen(header), 0);

    for (struct entry *np = content_list.lh_first; np; np = LIST_NEXT(np, entries)) {
        char *list = make_html_list(np->data, folder_name);
        send(client_fd, list, strlen(list), 0);
		free(list);
    }
	free(header);

    return;
}

