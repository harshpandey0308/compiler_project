#ifndef TOKENIZER_H
#define TOKENIZER_H

#define MAX_TOKENS 1000
#define MAX_TOKEN_SIZE 100
typedef enum TokenType{
    TOKEN_KEYWORD,              // keyword : int , float , char , if , else , for , while etc.
    TOKEN_IDENTIFIER,           // identifier should be start either from letters or from underscore( _ ) eg. token / _token / _token_12
    TOKEN_CONSTANT,            // Constants like integers numbers or float number , characters(characters reside within the single quoutes).
    TOKEN_STRING,              // String eg. "I AM ROCKY" or "TODAY IS SATURDAY" or "A".
    TOKEN_OPERATOR,            // OPERATORs {+ , - , * , / , % , < , > , == , = , <= , >= , && , || , ^}.
    TOKEN_SPECIAL_SYMBOL,       // Special Symbol includes ( , { , [ , ] , } , ) , "," , ";" 
    TOKEN_FUNCTION,
    TOKEN_CHARACTER,
    TOKEN_COMPARATOR
}TokenType;

typedef struct TOKEN{
    char lexeme[MAX_TOKEN_SIZE];
    TokenType tokentype;
}TOKEN;

typedef struct{
    TOKEN tokens[MAX_TOKENS];
    int token_count;
}TokenEntry;

void print_tokens(TokenEntry *token_table);

int lexer(const char* source[] , const int* n , TokenEntry *token_table);

#endif