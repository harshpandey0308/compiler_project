#ifndef PARSER_H
#define PARSER_H

#include"tokenizer.h"

typedef struct Node{
    char value[50];
    struct Node *left;
    struct Node *right;
    int is_Call;
    struct Node* ARG[50];
    int ARG_count;
    int is_addr_of;
    int is_deref;
    int is_keyword;
    int ptr_init;
}NODE;

NODE* create_node(char *exp);

void print(NODE* root);

int find_operator(TOKEN tokens[] , int start , int end);

NODE* Build_AST(TOKEN tokens[] , int start , int end);

//NODE* parser(const char* exp[] , const int* n);

#endif