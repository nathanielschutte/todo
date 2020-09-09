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

#define FILENAME_W      24
#define ITEM_TITLE_W    50
#define ITEM_DESC_W     200
#define ITEM_DATE_W     8
#define ITEM_TIME_W     4
#define ITEM_STATUS_W   16

#define LIST_ITEM_MAX   1024

#define CMD_NONE ""
#define CMD_HELP "help"
#define CMD_NEW "new"
#define CMD_LIST "ls"
#define CMD_ADD "add"
#define CMD_REMOVE "rm"

// List item
struct item_t {
    unsigned int id;                // task ID
    char title[ITEM_TITLE_W];       // name of task
    char desc[ITEM_DESC_W];         // description of task
    char date[ITEM_DATE_W];         // mm:dd:yyy
    char time[ITEM_TIME_W];         // hh:mm
    int flag;                       // important flag
    int status;                     // 0: unfinished, 1-99: WIP, 100: finished
    char status_str[ITEM_STATUS_W]; // status description
};

// List file data
struct todo_file_t {
    item_t list[LIST_ITEM_MAX];     // array of list items
    size_t item_count;              // count of actual items
    size_t items_finished;          // count of items finished
    size_t items_unfinished;        // count of items unfinished
    size_t items_wip;               // count of items in progress
}

// Argument values
struct arg_t {
    int use_file;
    char list[FILENAME_W];
} args;

DIR *dir;
FILE *list_file;

todo_file_t *todo;

char cmd_buf[8];
char file_buf[FILENAME_W + 6]; // + .todo

/**
 * Print usage
*/
void usage(char *progname) {
    printf("%s: manage your todo lists.\n", basename(progname));
    printf("\n==== COMMANDS ====\n");
    printf("  new       Create a new todo list\n");
    printf("  ls        List all todo lists in the current directory\n");
    printf("  add       Add an item to a list\n");
    printf("  rm        Remove an item from a list\n");
    printf("  help      Print help (this message)\n");

    printf("\n==== ITEM DATA FLAGS ====\n");
    printf("  -T [name]         item name (%u)\n", ITEM_TITLE_W);
    printf("  -m [desc]         item description (%u)\n", ITEM_DESC_W);
    printf("  -d [mm/dd/yyyy]   item due date\n");
    printf("  -t [mm:hh]        item due time\n");
    printf("  -p [status]       item completion status (0-100)\n");
    printf("  -f [flag]         item important flag\n");

    printf("\n==== OTHER FLAGS ====\n");
    printf("  -a [name] Specify the name of the list\n");
    printf("  -s        Sort the list by date-time\n");
    printf("  -I        Only show items flagged important\n");
    printf("  -x        Show extra list information\n");
}

void error(char *err) {
    printf("Error: %s", err);
    exit(1);
}

/**
 * Parse the commandline arguments
*/
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

/**
 * Find a .todo file in the current directory to use by default
*/
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

/**
 * Open the specified .todo file
*/
void open_todo_file(const char *file) {
    if (strcmp(EXT, get_ext(file))) {
            printf("Specified file %s does not have extension \'.todo\'\n", file);
            exit(1);
        }

    list_file = fopen(file, "r+");
    if (list_file == NULL) {
        printf("Failed to open todo file %s\n", file);
        exit(1);
    }
    printf("Using list %s\n", file);
}

/**
 * Decide to open default file, open specified file, or make new file
*/
void select_file(int c) {

    // file specified
    if (args.use_file) {
        strcpy(file_buf, args.list);
        file_special = 1;
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


// ###########################################
/**
 * List all .todo files in the current directory
*/
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

/**
 * Make a new list
*/
void list_add() {
    todo = malloc(sizeof(todo_file_t));
}


// ###########################################


/**
 * Main process
*/
void process(char *progname) {

    // assume reading a .todo file is needed
    int need_file = 1;

    // for required specified file
    int file_special = 0;

    if (!strcmp(CMD_LIST, cmd_buf)) {
        need_file = 0;
        list_todo_files();
    }
    else if (!strcmp(CMD_HELP, cmd_buf)) {
        need_file = 0;
        usage(progname);
    }
    else if (!strcmp(CMD_NEW), cmd_buf) {
        cmd_add();
    }

    // file needed to process command
    select_file(need_file);
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
    process(argv[0]);



    // CLEAN --------
    // close open file
    if (list_file != NULL) {
        fclose(list_file);
    }
    // free list memory
    if (todo != NULL) {
        free(todo);
    }

    return 0;
}