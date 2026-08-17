#ifndef PARSER_H
#define PARSER_H

#include"tokenizer.h"

#define LEXEME_SIZE 100
#define MAX_ARGUMENT 50
typedef enum{
    AST_IDENTIFIER,
    AST_OPERATOR,
    AST_FUNCTION,
    AST_FUNCTION_CALL,
    AST_DEREFERENCE,
    AST_STRING,
    AST_CHARACTER,
    AST_NUMBER,
    AST_ADDRESS_OF,
    AST_IF,
    AST_ELSE,
    AST_ELSEIF,
    AST_WHILE,
    AST_FOR,
    AST_INCREMENT,
    AST_RETURN,
    AST_BLOCK
}AST_NODE_TYPE;

typedef struct NODE NODE;
typedef struct FOR_NODE{
    struct NODE *init_node;
    struct NODE *cond_node;
    struct NODE *update_node;
}FOR_NODE;

typedef struct BODY{
    NODE *head;
    NODE *tail;
}BODY;

typedef struct NODE{
    char lexeme[LEXEME_SIZE];
    struct NODE *left;
    struct NODE *right;
    struct NODE *next;

    FOR_NODE *for_node;
    NODE *cond;
    BODY *body;

    struct NODE *ARG[MAX_ARGUMENT];
    int ARG_count;
    
    AST_NODE_TYPE type;

}NODE;

NODE* create_node(const char *lexeme , AST_NODE_TYPE type);

void print_AST(NODE* root);

int find_main_operator(const TokenEntry *token_table , int start , int end);

NODE* build_AST(const TokenEntry *token_table , int start , int end);

NODE *parse_cond(const TokenEntry *token_table , int *start);

NODE *parse_loop(const TokenEntry *token_table , int *start);

//NODE* parser(const char* exp[] , const int* n);

#endif