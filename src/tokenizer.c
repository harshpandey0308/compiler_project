#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"

TOKEN tokens[MAX_TOKENS];
int token_count = 0;

void print_tokens(TOKEN tokens[MAX_TOKENS] , int token_count){

    char *type_name[] = {"TOKEN_IDENTIFIER" , "TOKEN_OPERATOR" , "TOKEN_CONSTANT" , "TOKEN_SPECIAL_SYMBOL" , "TOKEN_KEYWORD","TOKEN_COMPARATOR" , "TOKEN_FUNCTION" , "TOKEN_STRING" , "TOKEN_CHARACTER"};

    for(int i=0 ; i<token_count ; i++){
        printf("\nToken %d : ",i);
        printf("%s  ->  %s\n",tokens[i].lexeme , type_name[tokens[i].tokentype]);
    }
}

int lexer_keyword(int *i , int *j , int *k , const char* source[] , char buffer[50]){
    while(isalnum(source[*i][*j]) || source[*i][*j] == '_'){
                    //printf("identifier checking.\n");
                    buffer[*k] = source[*i][*j];
                    (*j)++;
                    (*k)++;
                }
                //printf("identifier checked\n");
                buffer[*k] = '\0';

                //printf("the source is : %s\n",buffer);

                strcpy(tokens[token_count].lexeme , buffer);
                //printf("the lexeme of buffer is copied into tokens .\n");
                int peek = *j;

                while(source[*i][peek] == ' ') peek++;

                //printf("the lexeme of peek is %d and source is %c.\n",peek , source[i][peek]);


                if(source[*i][peek] == '(' && (strcmp(buffer , "if") != 0 && strcmp(buffer , "while") != 0 && strcmp(buffer , "for") != 0)){
                    tokens[token_count].tokentype = TOKEN_FUNCTION;
                }
                else if(strcmp(buffer , "int") == 0 || strcmp(buffer , "char") == 0 || 
                    strcmp(buffer , "float") == 0 || strcmp(buffer , "double") == 0 || 
                    strcmp(buffer , "if") == 0 || strcmp(buffer , "else") == 0 ||
                    strcmp(buffer , "while") == 0 || strcmp(buffer , "for") == 0 || strcmp(buffer , "return") == 0){
                        tokens[token_count].tokentype = TOKEN_KEYWORD;
                }
                else{
                    tokens[token_count].tokentype = TOKEN_IDENTIFIER;
                }
                token_count++;
}

int lexer(const char* source[] , const int* n){
    
    //printf("Lexical analysis started.\n");

    for(int i=0 ; i<*n ; i++){
        //printf("lexical analysis......\n");
        int j = 0;

        while(source[i][j] != '\0'){
            //printf("the source starts at %c.\n" ,source[i][j]);
            char buffer[50];
            int k = 0;

            if(isspace(source[i][j])){
                j++;
                continue;
            }
            else if(isalpha(source[i][j]) || source[i][j] == '_'){
                lexer_keyword(&i , &j , &k , source , &buffer);
            }

            else if(isdigit(source[i][j])){
                int has_dot = 0;
                while(isdigit(source[i][j]) || (source[i][j] == '.' && !has_dot))
                {
                    if(source[i][j] == '.') has_dot = 1;
                    buffer[k] = source[i][j];
                    j++;
                    k++;
                }
                buffer[k] = '\0';
                strcpy(tokens[token_count].lexeme , buffer);
                tokens[token_count].tokentype = TOKEN_CONSTANT;
                token_count++;

            //i--;
            }

            else if(source[i][j] == '+' || source[i][j] == '-' || source[i][j] == '*' || 
                    source[i][j] == '/' || source[i][j] == '%' || source[i][j] == '(' || source[i][j] == ')' || source[i][j] == '[' || source[i][j] == ']' || source[i][j] == '&'){
                buffer[0] = source[i][j++];
                buffer[1] = '\0';
                strcpy(tokens[token_count].lexeme , buffer);
                tokens[token_count].tokentype = TOKEN_OPERATOR;
                token_count++;

            }

            else if(source[i][j] == '"'){
                j++;
                int k=0;
                while(source[i][j] != '"' && source[i][j] != '\0'){
                    buffer[k++] = source[i][j++];
                }
                buffer[k] = '\0';
                j++;

                //printf("string token found : %s\n",buffer);
                //printf("NEXT char after string is %c\n",source[i][j]);

                strcpy(tokens[token_count].lexeme , buffer);
                tokens[token_count].tokentype = TOKEN_STRING;
                token_count++;
            }

            else if(source[i][j] == '\''){
                j++;
                while(source[i][j] != '\'' && source[i][j] != '\0'){
                    buffer[0] = source[i][j++];
                }
                buffer[1] = '\0';
                j++;

                strcpy(tokens[token_count].lexeme , buffer);
                tokens[token_count].tokentype = TOKEN_CHARACTER;
                token_count++;
            }

            else if(source[i][j] == ','){
                buffer[0] = source[i][j++];
                buffer[1] = '\0';
                j++;

                strcpy(tokens[token_count].lexeme , buffer);
                tokens[token_count].tokentype = TOKEN_SPECIAL_SYMBOL;
                token_count++;
            }

            else if(source[i][j] == '=' || source[i][j] == '<' || source[i][j] == '>' || source[i][j] == '!'){
                buffer[0] = source[i][j++];

                if(source[i][j] == '='){
                    buffer[1] = source[i][j++];
                    buffer[2] = '\0';
                }

                else{
                    buffer[1] = '\0';
                }

                strcpy(tokens[token_count].lexeme , buffer);

                if(strcmp(buffer , "==") == 0 || strcmp(buffer , "!=") == 0 || strcmp(buffer , "<") == 0 || strcmp(buffer , ">") == 0 || strcmp(buffer , "<=") == 0 || strcmp(buffer , ">=") == 0){
                    tokens[token_count].tokentype = TOKEN_COMPARATOR;
                }
                else{
                    tokens[token_count].tokentype = TOKEN_OPERATOR;
                }
                token_count++;
            }

            else if(source[i][j] == ';' || source[i][j] == '{' || source[i][j] == '}'){
                buffer[0] = source[i][j++];
                buffer[1] = '\0';
                strcpy(tokens[token_count].lexeme , buffer);
                //printf("Delimiter found : %s\n",buffer);
                tokens[token_count].tokentype = TOKEN_SPECIAL_SYMBOL;
                token_count ++;
            }

            else{
                printf("\n unknown character : %c \n", source[i][j]);
                j++;
            }
        }
    }

    print_tokens(tokens , token_count);

    //printf("Lexical analysis completed.\n");

    return token_count;

}