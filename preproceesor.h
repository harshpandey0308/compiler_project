#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#define MAX_DEFINES 100
#define MAX_LINES 200
#define MAX_LINE_LEN 200

typedef struct define{
    char name[50];
    char value[50];
}DEFINE;

extern DEFINE define_table[MAX_DEFINES];
extern int define_count;

void add_define(const char* name , const char* value);

void substitute(char* line , char* result);

int preprocesses(const char* filename , char lines[][MAX_LINE_LEN]);

#endif // PREPROCESSOR_