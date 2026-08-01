#include"parser.h"

#ifndef SEMANTIC_H
#define SEMANTIC_H


#define SYMBOL_TABLE_SIZE 200
#define SYMBOL_SIZE 50
#define SYMBOL_TYPE_SIZE 10
#define SCOPE_SIZE 50
typedef struct SYMBOL{
    char sym[SYMBOL_SIZE];
    char type[SYMBOL_TYPE_SIZE];
    char scope[SCOPE_SIZE];
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

typedef struct SymbolTable{
    SYMBOL sym_table[SYMBOL_TABLE_SIZE];
    int sym_count;
}SymbolTable;

extern char Current_Scope[SCOPE_SIZE];

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