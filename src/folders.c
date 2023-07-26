#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/socket.h>

extern char SERVING_DIR[];

// Singly linked list node
struct entry {
    char *data;
    SLIST_ENTRY(entry) entries; /* List */
};

SLIST_HEAD(slisthead, entry); // struct for head node

// puts folder name to title
// and returns html header
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

// strip out root folder from path using SERVING_DIR variable
char *strip_root_folder(const char *path) {
    char *stripped = malloc(strlen(path) * sizeof(char));
    int pos = (int)strlen(SERVING_DIR); //	starting position to copy
    int i = 0;
    while (path[pos] != '\0')
        stripped[i++] = path[pos++];

    stripped[i] = '\0'; // null termination
    return stripped;
}

// constructs html list with link to that file
char *make_html_list(const char *content, const char *root_folder) {
    char *list = malloc(BUFSIZ * sizeof(char));
    memset(list, 0, BUFSIZ);

    sprintf(list, "<li><a href=\"%s/%s\">%s</a></li>", root_folder, content,
            content);

    return list;
}

// puts all files in folder to list
void get_folder_contents(const char *folder_name,
                         struct slisthead *content_list) {
    struct dirent *dir_entry;
    DIR *dirp = opendir(folder_name); // directory pointer

    if (dirp == NULL) {
        perror("opendir");
        return;
    }

    // TODO skip . and ..

    // loop untill end of entry
    while ((dir_entry = readdir(dirp))) {
        struct entry *content_node =
            malloc(sizeof(struct entry)); // create new node
        content_node->data = malloc(BUFSIZ * sizeof(char));
        memset(content_node->data, 0, BUFSIZ); // initialize to 0

        // put current file entry to node
        strncat(content_node->data, dir_entry->d_name,
                strlen(dir_entry->d_name));
        // insert current node to list
        SLIST_INSERT_HEAD(content_list, content_node, entries);
    }
    closedir(dirp);
}

void send_folder_content(const char *folder_name, int client_fd) {
    char *root_folder = strip_root_folder(folder_name);
    char *header = make_header(folder_name);

    // TODO check if exist index.html file

    // send html header to client
    send(client_fd, header, strlen(header), 0);

    struct slisthead content_list; // linked list
    SLIST_INIT(&content_list);     // initialize head node

    // load all the folder contents
    get_folder_contents(folder_name, &content_list);

    // loop through each entry in content list
    struct entry *np;
    SLIST_FOREACH(np, &content_list, entries) {
        char *list = make_html_list(np->data, root_folder); // make html list
        send(client_fd, list, strlen(list), 0); // send list to client
        free(list);
    }
    // cleanup
    SLIST_INIT(&content_list);
    free(header);
    free(root_folder);

    return;
}
