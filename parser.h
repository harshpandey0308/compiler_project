#ifndef PARSER_H
#define PARSER_H

#include"tokenizer.h"

typedef struct Node{
    char value[50];
    struct Node *left;
    struct Node *right;
}NODE;

NODE* create_node(char *exp);

void print(NODE* root);

int find_operator(TOKEN tokens[] , int start , int end);

NODE* Build_AST(TOKEN tokens[] , int start , int end);

//NODE* parser(const char* exp[] , const int* n);

#endif