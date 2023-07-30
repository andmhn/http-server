#include "utils.h"
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

    char *css =
        "body{max-width:650px;margin:40px auto;padding:0 10px;font:18px/1.5 "
        "-apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, \"Helvetica "
        "Neue\", Arial, \"Noto Sans\", sans-serif, \"Apple Color Emoji\", "
        "\"Segoe UI Emoji\", \"Segoe UI Symbol\", \"Noto Color "
        "Emoji\";color:#444}h1,h2,h3{line-height:1.2}@media "
        "(prefers-color-scheme: "
        "dark){body{color:#c9d1d9;background:#0d1117}a:link{color:#58a6ff}a:"
        "visited{color:#8e96f0}}";

    char *format = "<!DOCTYPE html>\
<head>\
	<title>Listing files in: %s</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
	<style>%s</style>\
</head>\
<h1>Listing Files in %s</h1>\
<h2><a href=\"..\" style = \"text-decoration: none\">..</a></h2>\
\
";

    sprintf(header, format, folder, css, folder);

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
char *make_html_list(const char *content, char *folder) {
    char *list = malloc(BUFSIZ * sizeof(char));
    memset(list, 0, BUFSIZ);

    sprintf(list, "<li><a href=\"%s%s\">%s</a></li>", folder, content, content);
    return list;
}

// puts all files in folder to list
void get_folder_contents(const char *folder_name, struct slisthead *dir_list,
                         struct slisthead *file_list) {
    struct dirent *dir_entry;
    DIR *dirp = opendir(folder_name); // directory pointer

    if (dirp == NULL) {
        perror("opendir");
        return;
    }

    // loop untill end of entry
    while ((dir_entry = readdir(dirp))) {
        // skip . and ..
        if (!strncmp(dir_entry->d_name, ".", 1) ||
            !strncmp(dir_entry->d_name, "..", 2))
            continue;

        struct entry *node = malloc(sizeof(struct entry)); // create new node
        node->data = malloc(BUFSIZ * sizeof(char));
        memset(node->data, 0, BUFSIZ); // initialize to 0

        // put current file entry to node
        strncat(node->data, dir_entry->d_name, strlen(dir_entry->d_name));

        char path[BUFSIZ];
        sprintf(path, "%s%s", folder_name, dir_entry->d_name);
        // insert current node to list

        if (is_dir(path)) {
            strncat(node->data, "/", 1);
            SLIST_INSERT_HEAD(dir_list, node, entries);
        } else
            SLIST_INSERT_HEAD(file_list, node, entries);
    }
    closedir(dirp);
}

void send_list(struct slisthead *list, char *curr_folder, int sock) {
    // loop through each entry in content list
    struct entry *np;
    SLIST_FOREACH(np, list, entries) {
        char *html_list =
            make_html_list(np->data, curr_folder);   // make html list
        send(sock, html_list, strlen(html_list), 0); // send list to client
        free(html_list);
    }
}

void send_folder_content(char *folder_name, int client_fd) {
    if (folder_name[strlen(folder_name) - 1] != '/') {
        strncat(folder_name, "/", 1);
    }
    char *curr_folder = strip_root_folder(folder_name);
    char *header = make_header(curr_folder);
    // send html header to client
    send(client_fd, header, strlen(header), 0);

    struct slisthead dir_list, file_list; // linked lists
    SLIST_INIT(&dir_list);                // initialize head node
    SLIST_INIT(&file_list);               // initialize head node

    // load all the folder contents
    get_folder_contents(folder_name, &dir_list, &file_list);
    // serve it
    send_list(&dir_list, curr_folder, client_fd);
    send_list(&file_list, curr_folder, client_fd);

    // cleanup
    SLIST_INIT(&dir_list);
    SLIST_INIT(&file_list);
    free(header);
    free(curr_folder);

    return;
}
