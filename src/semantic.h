#include"parser.h"

#ifndef SEMANTIC_H
#define SEMANTIC_H


#define SYMBOL_TABLE_SIZE 200
#define SYMBOL_SIZE 50
#define SCOPE_SIZE 50



typedef enum{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_CHAR,
    TYPE_VOID,
    UNKNOWN_TYPE
}DataType;
typedef struct SYMBOL{
    char sym[SYMBOL_SIZE];
    DataType type;
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
    SYMBOL table[SYMBOL_TABLE_SIZE];
    int sym_count;
}SymbolTable;

typedef struct{
    SymbolTable symbols;
    char current_scope[SCOPE_SIZE];
    char Current_function[SYMBOL_SIZE];
}Semantic_ctxt;

int is_float(const char* s);

int get_slot(const SymbolTable *table , const char *name , const char *scope);

void add_symbol(Semantic_ctxt *context , const char *name , DataType type , int is_param , int size);

void Check_Undeclared(NODE* root ,Semantic_ctxt *context);

DataType get_type(NODE* node , Semantic_ctxt *context);

void Type_check(NODE* root , Semantic_ctxt *context);

int is_declared(const char* line);

DataType stringtotype(const char type[20]);

void parse_declaration(const char* line , Semantic_ctxt *context);

void BUILD_SYMBOL_TEXT(const SymbolTable *table , char *buffer);

const char *Symbol_Text(char *buffer);

void free_tree(NODE* root);

//void semantic(const char* exp[] , NODE* root , const int* n);

#endif