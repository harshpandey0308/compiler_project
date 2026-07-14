#include"parser.h"

#ifndef SEMANTIC_H
#define SEMANTIC_H



typedef struct symbol{
    char sym[50];
    char type[10];
    char scope[50];
    union 
    {
        int ival;
        float fval;
        char cval;
        double dval;
    }value;
    int is_initialized;
    int is_param;
    int size;
}SYMBOL;

extern SYMBOL sym_table[50];
extern int sym_count;
extern char Current_Scope[50];

int is_float(const char* s);

int get_slot(char *name , char *scope);

void add_symbol(const char* name , const char* type , char* Current_Scope , int is_param , int size);

void Check_Undeclared(NODE* root , char* Current_Scope);

char* get_type(NODE* node , char* Current_Scope);

void Type_check(NODE* root , char* Current_Scope);

int is_declared(const char* line);

void parse_declaration(const char* line);

void print_sym();

void free_tree(NODE* root);

//void semantic(const char* exp[] , NODE* root , const int* n);

#endif