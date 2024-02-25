#include "window.h"

#include <cerrno>
#include <cstring>
#include <string>
#include <stdio.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

Window_Main win;
Fl_Text_Buffer *buff;
std::thread *server_thread;

//// magic
//// set port
//// set IS_RUNNING flag line 88 while loop

extern "C"
{
    // definitions from server
    extern bool IS_RUNNING;
    extern char PORT[];
    int init(void);
    bool has_permission(const char *filename);
    bool is_dir(const char *path);

    void log_init(FILE * file)
    {
        // do nothing
    }

    // output to buffer 
    void log_msg(const char* msg)
    {
        buff->append(msg);
        buff->append("\n");
        win.output->scroll(buff->length(), 0);
    }
    // strerror to buffer
    void log_perr(char* err)
    {
        char fmt[BUFSIZ] = {0};
        sprintf(fmt, "[ERR]: %s : %s\n", err, strerror(errno));

        buff->append(fmt);
        win.output->scroll(buff->length(), 0);
    }
}

bool is_valid_input()
{
    // check empty values
    if(strlen(win.folder_input->value()) < 1) return false;
    if(strlen(win.port_input->value()) < 1) return false;

    // check valid folder
    if (is_dir(win.folder_input->value())) {
        if (!has_permission(win.folder_input->value())) {
            log_msg("[ERR]: Access Denied");
            return false;
        }
    } else // invalid folder
    {
        log_msg("[ERR]: Invalid Folder");
        return false;
    }

    // TODO check port

    return true;
}

void start_server()
{   
    if(!is_valid_input()) return;

    // initialize
    chdir(win.folder_input->value());
    strncpy(PORT,win.port_input->value(), 4);

    char msg[BUFSIZ]= {0};
    sprintf(msg, "Serving in: %s\nLocal URL: http://127.0.0.1:%s",
            win.folder_input->value(), win.port_input->value());
    log_msg(msg);
 
    IS_RUNNING = 1;
    server_thread = new std::thread(init);
    win.start_btn->hide();
    win.stop_btn->show();
}

void clear_output()
{
    // empty the output box
    buff->text("");
}

void stop_server()
{
    if(!IS_RUNNING) return;
    IS_RUNNING = 0;
    server_thread->detach();

    // clean the listening socket
    char cmd[BUFSIZ/4];
    sprintf(cmd, "curl -s --head http://127.0.0.1:%s/ > /dev/null", PORT);
    system(cmd);

    win.stop_btn->hide();
    win.start_btn->show();
    log_msg("[INFO]: Server Stopped!\n");
}

int main(int argc, char **argv)
{
    char cwd[BUFSIZ] = {0};
    getcwd(cwd, BUFSIZ);

    buff = new Fl_Text_Buffer();
    Fl_Text_Display *disp = win.output;
    disp->buffer(buff); // attach text buffer

    win.show();
    win.folder_input->value(cwd);
    win.port_input->value("8080");
    Fl::run();
}
