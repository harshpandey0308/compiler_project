#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>


typedef enum{
    IDENTIFIER,
    NUMBER,
    OPERATOR
}Tokentype;

typedef struct{
    char value[50];
    Tokentype type;
}Token;

typedef struct Node{
    char value[50];
    struct Node *left;
    struct Node *right;
}NODE;

NODE *create_node(char *exp){
    NODE* new = (NODE*)malloc(sizeof(NODE));
    strcpy(new->value,exp);
    new->left = NULL;
    new->right = NULL;
    return new;
}

void print(NODE* root){
    if(root == NULL){
        return;
    }
    print(root->left);
    printf("%s ",root->value);
    print(root->right);
}

int main(){
    char exp[] = "a = b + 45;";
    Token tokens[50];
    int token_count = 0;

    for(int i=0 ; exp[i]!='\0' ; i++){
        char buffer[50];
        int j = 0;
        
        if(isalpha(exp[i]) || exp[i] == '_'){

            while(isalnum(exp[i]) || exp[i] == '_'){
                buffer[j++] = exp[i++];
            }
            buffer[j] = '\0';

            tokens[token_count].type = IDENTIFIER;
            strcpy(tokens[token_count].value , buffer);
            token_count++;

            i--;
        }
        
        else if(isdigit(exp[i])){
            while(isdigit(exp[i])){
                buffer[j++] = exp[i++];
            }
            buffer[j] = '\0';
            
            tokens[token_count].type = NUMBER;
            strcpy(tokens[token_count].value , buffer);
            token_count++;

            i--;
        }

        else if(exp[i] == '+' || exp[i] == '-' || exp[i] == '*' || exp[i] == '/' || exp[i] == '%' || exp[i] == ';' || exp[i] == '='){
            buffer[0] = exp[i];
            buffer[1] = '\0';

            tokens[token_count].type = OPERATOR;
            strcpy(tokens[token_count].value,buffer);
            token_count++;
        }

        else if(exp[i] == ' '){
            continue;
        }

        else{
            printf("UNKNOWN CHARACTER:%c.\n",exp[i]);
        }
    }

    NODE* root = create_node(tokens[1].value);
    root->left = create_node(tokens[0].value);
    NODE* plus = create_node(tokens[3].value);
    plus->left = create_node(tokens[2].value);
    plus->right = create_node(tokens[4].value);

    root->right = plus;
    
    NODE* temp = root;
    print(temp);

    return 0;
}