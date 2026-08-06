#ifndef PARSER_H
#define PARSER_H

#include"tokenizer.h"

#define LEXEME_SIZE 100
#define MAX_ARGUMENT 50
typedef enum{
    AST_IDENTIFIER,
    AST_OPERATOR,
    AST_FUNCTION_CALL,
    AST_DEREFERENCE,
    AST_STRING,
    AST_CHARACTER,
    AST_NUMBER,
    AST_ADDRESS_OF,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_RETURN,
    AST_BLOCK
}AST_NODE_TYPE;

typedef struct NODE{
    char lexeme[LEXEME_SIZE];
    struct NODE *left;
    struct NODE *right;
    struct NODE *next;
    struct NODE* ARG[MAX_ARGUMENT];
    int ARG_count;
    AST_NODE_TYPE type;
}NODE;

typedef struct BLOCK{
    struct NODE *head;
    struct NODE *tail;
}BLOCK;


NODE* create_node(const char *lexeme , AST_NODE_TYPE type);

void print_AST(NODE* root);

int find_main_operator(const TokenEntry *token_table , int start , int end);

NODE* build_AST(const TokenEntry *token_table , int start , int end);



//NODE* parser(const char* exp[] , const int* n);

#endif