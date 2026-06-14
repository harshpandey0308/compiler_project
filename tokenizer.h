#ifndef TOKENIZER_H
#define TOKENIZER_H

typedef enum type{
    IDENTIFIER,
    OPERATOR,
    NUMBER,
    DELIMITER,
    KEYWORD,
    COMPARATORS
}token_type;



typedef struct token{
    char value[50];
    token_type tokentype;
}TOKEN;

extern TOKEN tokens[500];
extern int token_count;

void print_tokens(TOKEN tokens[500] , int token_count);

int lexer(const char* exp[] , const int* n);

#endif