#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"

TOKEN tokens[500];
int token_count = 0;

void print_tokens(TOKEN tokens[500] , int token_count){

    char *type_name[] = {"IDENTIFIER" , "OPERATOR" , "NUMBER" , "DELIMITER" , "KEYWORD","COMPARATORS" , "FUNC_NAME"};

    for(int i=0 ; i<token_count ; i++){
        printf("\nToken %d : ",i);
        printf("%s  ->  %s\n",tokens[i].value , type_name[tokens[i].tokentype]);
    }
}

int lexer(const char* exp[] , const int* n){
    
    printf("Lexical analysis started.\n");

    for(int i=0 ; i<*n ; i++){
        //printf("lexical analysis......\n");
        int j = 0;

        while(exp[i][j] != '\0'){
            //printf("the exp starts at %s.\n" ,exp);
            char buffer[50];
            int k = 0;

            if(isspace(exp[i][j])){
                j++;
                continue;
            }
            else if(isalpha(exp[i][j]) || exp[i][j] == '_'){
                while(isalnum(exp[i][j]) || exp[i][j] == '_'){
                    //printf("identifier checking.\n");
                    buffer[k] = exp[i][j];
                    //printf("%s\n",exp);
                    j++;
                    k++;
                }
                //printf("identifier checked\n");
                buffer[k] = '\0';

                strcpy(tokens[token_count].value , buffer);
                //printf("the value of buffer is copied into tokens .\n");
                int peek = j;

                while(exp[i][peek] == ' ') peek++;

                //printf("the value of peek is %d and exp is %c.\n",peek , exp[i][peek]);


                if(exp[i][peek] == '(' && (strcmp(buffer , "if") != 0 && strcmp(buffer , "while") != 0 && strcmp(buffer , "for") != 0)){
                    tokens[token_count].tokentype = FUNC_NAME;
                }
                else if(strcmp(buffer , "int") == 0 || strcmp(buffer , "char") == 0 || 
                    strcmp(buffer , "float") == 0 || strcmp(buffer , "double") == 0 || 
                    strcmp(buffer , "if") == 0 || strcmp(buffer , "else") == 0 ||
                    strcmp(buffer , "while") == 0 || strcmp(buffer , "for") == 0 || strcmp(buffer , "return") == 0){
                        tokens[token_count].tokentype = KEYWORD;
                }
                else{
                    tokens[token_count].tokentype = IDENTIFIER;
                }
                token_count++;
            
            }

            else if(isdigit(exp[i][j])){
                int has_dot = 0;
                while(isdigit(exp[i][j]) || (exp[i][j] == '.' && !has_dot))
                {
                    if(exp[i][j] == '.') has_dot = 1;
                    buffer[k] = exp[i][j];
                    j++;
                    k++;
                }
                buffer[k] = '\0';
                strcpy(tokens[token_count].value , buffer);
                tokens[token_count].tokentype = NUMBER;
                token_count++;

            //i--;
            }

            else if(exp[i][j] == '+' || exp[i][j] == '-' || exp[i][j] == '*' || exp[i][j] == '/' || exp[i][j] == '%' || exp[i][j] == '(' || exp[i][j] == ')'){
                buffer[0] = exp[i][j++];
                buffer[1] = '\0';
                strcpy(tokens[token_count].value , buffer);
                tokens[token_count].tokentype = OPERATOR;
                token_count++;

            }

            else if(exp[i][j] == '=' || exp[i][j] == '<' || exp[i][j] == '>' || exp[i][j] == '!'){
                buffer[0] = exp[i][j++];

                if(exp[i][j] == '='){
                    buffer[1] = exp[i][j++];
                    buffer[2] = '\0';
                }

                else{
                    buffer[1] = '\0';
                }

                strcpy(tokens[token_count].value , buffer);

                if(strcmp(buffer , "==") == 0 || strcmp(buffer , "!=") == 0 || strcmp(buffer , "<") == 0 || strcmp(buffer , ">") == 0 || strcmp(buffer , "<=") == 0 || strcmp(buffer , ">=") == 0){
                    tokens[token_count].tokentype = COMPARATORS;
                }
                else{
                    tokens[token_count].tokentype = OPERATOR;
                }
                token_count++;
            }

            else if(exp[i][j] == ';' || exp[i][j] == '{' || exp[i][j] == '}'){
                buffer[0] = exp[i][j++];
                buffer[1] = '\0';
                strcpy(tokens[token_count].value , buffer);
                //printf("Delimiter found : %s\n",buffer);
                tokens[token_count].tokentype = DELIMITER;
                token_count ++;
            }

            else{
                printf("\n unknown character : %c \n", exp[i][j]);
                j++;
            }
        }
    }

    print_tokens(tokens , token_count);

    printf("Lexical analysis completed.\n");

    return token_count;

}