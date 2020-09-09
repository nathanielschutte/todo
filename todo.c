
#include <stdio.h>

#define STAT_UNFINISHED 0
#define STAT_FINISHED 1
#define STAT_WIP 2

// List item
struct item_t {
    unsigned char id;
    char title[50];
    char desc[200];
    char date[8];
    char time[4];
    int flag;
    int status;
};

enum CMD {
    NEW,
    ADD,
    REMOVE,
    VIEW,
    ORDER,
};

int main() {
    printf("hello world!");

    return 0;
}