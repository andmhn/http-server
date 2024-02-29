#include "utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// puts folder name to title
// and returns html header
char *make_header(const char *folder) {
    char *header = (char*) malloc(BUFSIZ);

    char css[] =
        "body{max-width:650px;margin:40px auto;padding:0 10px;font:18px/1.5 "
        "-apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, \"Helvetica "
        "Neue\", Arial, \"Noto Sans\", sans-serif, \"Apple Color Emoji\", "
        "\"Segoe UI Emoji\", \"Segoe UI Symbol\", \"Noto Color "
        "Emoji\";color:#444}h1,h2,h3{line-height:1.2}@media "
        "(prefers-color-scheme: "
        "dark){body{color:#c9d1d9;background:#0d1117}a:link{color:#58a6ff}a:"
        "visited{color:#8e96f0}}";

    char format[] =
        "<!DOCTYPE html>"
        "<head>"
        "<title>Listing files in: %s</title>"
        "<meta name=\"viewport\"content=\"width=device-width,initial-scale=1\">"
        "<meta charset=\"UTF-8\">"
        "<style>%s</style>"
        "</head>"
        "<h1>Listing Files in %s</h1>"
        "<h1><a href=\"..\" style = \"text-decoration: none\">..</a></h1>";

    sprintf(header, format, folder, css, folder);

    return header;
}

// create lists out of name and sends it to socket
void send_list(char *content, size_t size, int sock) {
    char html_list[100 + (2 * size)]; // 100 for html tags

    sprintf(html_list, "<li><a href=\"%s\">%s</a></li>", content, content);

    send(sock, html_list, strlen(html_list), 0); // send list to client
}

// serves folder contents as lists in html
static void serve_folder_contents(const char *folder_name, int sock) {
    struct dirent *dir_entry;
    DIR *dirp = opendir(folder_name); // directory pointer

    // loop untill end of entry
    while ((dir_entry = readdir(dirp))) {
        // skip . and ..
        if (!strncmp(dir_entry->d_name, ".", 1) ||
            !strncmp(dir_entry->d_name, "..", 2))
            continue;

        size_t entry_size = strlen(dir_entry->d_name);

        // check folder
        char filepath[BUFSIZ] = {0};
        strncpy(filepath, folder_name, strlen(folder_name));
        strncat(filepath, dir_entry->d_name, entry_size);

        if (is_dir(filepath)) {
            memset(filepath, 0, BUFSIZ); // it will contain folder basename
            strncpy(filepath, dir_entry->d_name, entry_size);
            strcat(filepath, "/");
            send_list(filepath, entry_size + 1, sock);
        }
    }
    closedir(dirp);
    dirp = opendir(folder_name); // redo it for files
    while ((dir_entry = readdir(dirp))) {
        char filepath[BUFSIZ] = {0};
        strncpy(filepath, folder_name, strlen(folder_name));
        strncat(filepath, dir_entry->d_name, strlen(dir_entry->d_name));

        if (!is_dir(filepath))
            send_list(dir_entry->d_name, strlen(dir_entry->d_name), sock);
    }
    closedir(dirp);
}

void send_folder_content(char *folder_name, int client_fd) {
    if (folder_name[strlen(folder_name) - 1] != '/') {
        strcat(folder_name, "/");
    }

    char *header = make_header(folder_name);
    // send html header to client
    send(client_fd, header, strlen(header), 0);

    // serve all the folder contents
    serve_folder_contents(folder_name, client_fd);

    // cleanup
    free(header);
}
