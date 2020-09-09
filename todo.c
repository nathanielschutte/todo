#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <libgen.h>
#include <unistd.h>

#define EXT "todo"

#define STAT_UNFINISHED "unfinished"
#define STAT_FINISHED "WIP"
#define STAT_WIP "done"

#define FILENAME_W 24

#define CMD_NONE ""
#define CMD_HELP "help"
#define CMD_NEW "new"
#define CMD_LIST "ls"
#define CMD_ADD "add"
#define CMD_REMOVE "rm"

// List item
struct item_t {
    unsigned char id;       // task ID
    char title[50];         // name of task
    char desc[200];         // description of task
    char date[8];           // mm:dd:yyy
    char time[4];           // hh:mm
    int flag;               // important flag
    int status;             // 0: unfinished, 1-99: WIP, 100: finished
    char status_str[16];    // status description
};

// Argument values
struct arg_t {
    int use_file;
    char list[FILENAME_W];
} args;

DIR *dir;
FILE *list_file;

char cmd_buf[8];
char file_buf[FILENAME_W + 6]; // + .todo

void usage(char *progname) {
    printf("%s: manage your todo lists.\n", basename(progname));
    printf("\n==== COMMANDS ====\n");
    printf("  help      Print help (this message)\n");
    printf("  new       Create a new todo list\n");
    printf("  ls        List all todo lists in the current directory\n");
    printf("  add       Add an item to a list\n");
    printf("  rm        Remove an item from a list\n");

    printf("\n==== FLAGS ====\n");
    printf("  -a [name] Specify the name of the list\n");
}

void error(char *err) {
    printf("Error: %s", err);
    exit(1);
}

void parse_args(int argc, char **argv) {
    long opt;
    while ((opt = getopt(argc, argv, "a:")) != -1) {
        switch (opt) {
            case 'a':
            if (strlen(optarg) >= FILENAME_W) {
                printf("Filename must be <= %u characters", FILENAME_W);
                exit(1);
            }
            strcpy(args.list, optarg);
            args.use_file = 1;
            break;
        }
    }
}

const char *get_ext(const char *filename) {
    const char *dot = strchr(filename, '.');
    if (! dot || dot == filename) return "";
    return dot + 1;
}

void list_todo_files() {
    struct dirent *file;

    // open current directory
    dir = opendir(".");
    if (dir == NULL) {
        printf("Unable to read directory\n");
        return;
    }
    while ((file = readdir(dir))) {
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
            continue;
        if (!strcmp(EXT, get_ext(file->d_name))) {
            printf("%s\n", file->d_name);
        }
    }
    closedir(dir);
}


char *seek_todo_file() {
    struct dirent *file;

    // open current directory
    dir = opendir(".");
    if (dir == NULL) {
        printf("Unable to read directory\n");
        return NULL;
    }
    while ((file = readdir(dir))) {
        if (!strcmp(file->d_name, ".") || !strcmp(file->d_name, ".."))
            continue;
        if (!strcmp(EXT, get_ext(file->d_name))) {
            closedir(dir);
            return file->d_name;
        }
    }


    closedir(dir);
    return NULL;
}

void open_todo_file(const char *file) {
    if (0 != strcmp(EXT, get_ext(args.list))) {
            printf("Specified file %s does not have extension \'.todo\'\n", file);
            exit(1);
        }

    list_file = fopen(args.list, "r+");
    if (list_file == NULL) {
        printf("Failed to open todo file %s\n", file);
        exit(1);
    }
}

void process() {

    int need_file = 1;

    if (!strcmp(CMD_LIST, cmd_buf)) {
        need_file = 0;
        list_todo_files();
    }
    else if (!strcmp(CMD_NONE, cmd_buf)) {

    }


    // file needed to process command
    if (need_file) {

        // file specified
        if (args.use_file) {
            strcpy(file_buf, args.list);
        }
        // unspecified
        else {
            char *list = seek_todo_file();
            if (list == NULL) {
                printf("Could not locate a todo file in current directory\n");
                exit(1);
            }
            strcpy(file_buf, list);
        }

        open_todo_file(file_buf);
    }
}



int main(int argc, char **argv) {

    // get arguments
    args.use_file = 0;
    parse_args(argc, argv);

    // command specified
    if (argc > 1) {
        strcpy(cmd_buf, argv[1]);
    }
    // just "todo": display default todo list
    else {
        strcpy(cmd_buf, CMD_NONE);
    }

    // main process
    process();


    // clean
    if (list_file != NULL) {
        fclose(list_file);
    }

    return 0;
}