#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"


void print_tokens(TokenEntry *token_table){
    printf("the total token found = %d\n", token_table->token_count);

    char *type_name[] = {"TOKEN_KEYWORD" , "TOKEN_IDENTIFIER" , "TOKEN_CONSTANT" , "TOKEN_STRING" , "TOKEN_OPERATOR" , "TOKEN_SPECIAL_SYMBOL" , "TOKEN_FUNCTION" , "TOKEN_CHARACTER" , "TOKEN_COMPARATOR"};

    for(int i=0 ; i<token_table->token_count ; i++){
        printf("\nToken %d : ",i);
        printf("%s  ->  %s\n",token_table->tokens[i].lexeme , type_name[token_table->tokens[i].tokentype]);
    }
}



int lexer(const char* source[] , const int* n , TokenEntry *token_table){
    
    printf("Lexical analysis started.\n");

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
                while(isalnum(source[i][j]) || source[i][j] == '_'){
                    //printf("identifier checking.\n");
                    buffer[k] = source[i][j];
                    (j)++;
                    (k)++;
                }
                //printf("identifier checked\n");
                buffer[k] = '\0';

                //printf("the source is : %s\n",buffer);
                //printf("token count = %d.\n",token_table->token_count);

                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                //printf("the lexeme of buffer is copied into tokens .\n");
                int peek = j;

                while(source[i][peek] == ' ') peek++;

                //printf("the lexeme of peek is %d and source is %c.\n",peek , source[i][peek]);


                if(source[i][peek] == '(' && (strcmp(buffer , "if") != 0 && strcmp(buffer , "while") != 0 && strcmp(buffer , "for") != 0)){
                    token_table->tokens[token_table->token_count].tokentype = TOKEN_FUNCTION;
                }
                else if(strcmp(buffer , "int") == 0 || strcmp(buffer , "char") == 0 || 
                    strcmp(buffer , "float") == 0 || strcmp(buffer , "double") == 0 || 
                    strcmp(buffer , "if") == 0 || strcmp(buffer , "else") == 0 ||
                    strcmp(buffer , "while") == 0 || strcmp(buffer , "for") == 0 || strcmp(buffer , "return") == 0){
                        token_table->tokens[token_table->token_count].tokentype = TOKEN_KEYWORD;
                        //printf("the token = %s of token type = %d\n",tokens[token_count].lexeme , tokens[token_count].tokentype);
                }
                else{
                    token_table->tokens[token_table->token_count].tokentype = TOKEN_IDENTIFIER;
                }
                token_table->token_count++;
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
                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                token_table->tokens[token_table->token_count].tokentype = TOKEN_CONSTANT;
                token_table->token_count++;

            //i--;
            }

            else if(source[i][j] == '+' || source[i][j] == '-' || source[i][j] == '*' || 
                    source[i][j] == '/' || source[i][j] == '%' || source[i][j] == '&'){
                buffer[0] = source[i][j++];
                buffer[1] = '\0';
                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                token_table->tokens[token_table->token_count].tokentype = TOKEN_OPERATOR;
                token_table->token_count++;

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

                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                token_table->tokens[token_table->token_count].tokentype = TOKEN_STRING;
                token_table->token_count++;
            }

            else if(source[i][j] == '\''){
                j++;
                while(source[i][j] != '\'' && source[i][j] != '\0'){
                    buffer[0] = source[i][j++];
                }
                buffer[1] = '\0';
                j++;

                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                token_table->tokens[token_table->token_count].tokentype = TOKEN_CHARACTER;
                token_table->token_count++;
            }

            else if(source[i][j] == ','){
                buffer[0] = source[i][j++];
                buffer[1] = '\0';

                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                token_table->tokens[token_table->token_count].tokentype = TOKEN_SPECIAL_SYMBOL;
                token_table->token_count++;
            }

            else if(source[i][j] == '=' || source[i][j] == '<' || source[i][j] == '>' || source[i][j] == '!'){
                buffer[0] = source[i][j++];
                //printf("buffer : %c\n",buffer[0]);
                //printf("tokens = %c\n", source[i][j]);

                if(source[i][j] == '='){
                    buffer[1] = source[i][j++];
                    buffer[2] = '\0';
                    //printf("buffer = %s\n",buffer);
                }

                else{
                    buffer[1] = '\0';
                }

                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);

                if(strcmp(buffer , "==") == 0 || strcmp(buffer , "!=") == 0 || strcmp(buffer , "<") == 0 || strcmp(buffer , ">") == 0 || strcmp(buffer , "<=") == 0 || strcmp(buffer , ">=") == 0){
                    token_table->tokens[token_table->token_count].tokentype = TOKEN_COMPARATOR;
                }
                else{
                    token_table->tokens[token_table->token_count].tokentype = TOKEN_OPERATOR;
                }
                token_table->token_count++;
            }

            else if(source[i][j] == ';' || source[i][j] == '{' || source[i][j] == '}' || source[i][j] == '(' || source[i][j] == ')'){
                buffer[0] = source[i][j++];
                buffer[1] = '\0';
                strcpy(token_table->tokens[token_table->token_count].lexeme , buffer);
                //printf("Delimiter found : %s\n",buffer);
                token_table->tokens[token_table->token_count].tokentype = TOKEN_SPECIAL_SYMBOL;
                token_table->token_count ++;
            }

            else{
                printf("\n unknown character : %c \n", source[i][j]);
                j++;
            }
        }
    }

    print_tokens(token_table);

    printf("Lexical analysis completed.\n");

    return token_table->token_count;

}