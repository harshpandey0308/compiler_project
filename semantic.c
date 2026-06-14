#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"parser.h"
#include"tokenizer.h"
#include"semantic.h"

SYMBOL sym_table[50];
int sym_count = 0;
char Current_Scope[50] ;


int is_float(const char* s){
    for(int i=0 ; s[i] != '\0' ; i++){
        if(s[i] == '.') return 1;
    }
    return 0;
}

void add_symbol(const char* name , const char* type){
    for(int i=0 ; i<sym_count ; i++){
        if(strcmp(sym_table[i].sym , name) == 0){
            printf("ERROR : The %s is already declared.\n",name);
            return;
        }
    }

    strcpy(sym_table[sym_count].sym , name);
    strcpy(sym_table[sym_count].type , type);
    sym_table[sym_count].is_initialized = 0;

    sym_count++;
}

void Check_Undeclared(NODE* root , char* Current_Scope){
    if(root == NULL){
        return;
    }
    if(isalpha(root->value[0]) || root->value[0] == '_'){
        int found = 0;
        for(int i=0 ; i<sym_count ; i++){
            if(strcmp(sym_table[i].sym , root->value) == 0 && strcmp(sym_table[i].scope , Current_Scope) == 0){
                found = 1;
                break;
            }
        }
        if(!found){
            printf("ERROR , %s is used , but not declared.\n",root->value);
        }
    }

    Check_Undeclared(root->left , Current_Scope);
    Check_Undeclared(root->right , Current_Scope);
}

char* get_type(NODE* node , char* Current_Scope){
    if(node == NULL) return "UNKNOWN";

    if(node->left == NULL && node->right == NULL){
        if(isdigit(node->value[0])){
            if(is_float(node->value)) return "float";
            else return "int";
        }

        for(int i=0 ; i<sym_count ; i++){
            if(strcmp(sym_table[i].sym , node->value) == 0 && strcmp(sym_table[i].scope , Current_Scope) == 0){
                return sym_table[i].type;
            }
        }
        return "UNKNOWN";
    }

    char* left_type = get_type(node->left , Current_Scope);
    char* right_type = get_type(node->right , Current_Scope);

    if(strcmp(left_type , "float")==0 || strcmp(right_type,"float")==0){
        return "float";
    }
    return "int";
}

void Type_check(NODE* root){
    if(root == NULL) return ;

    if(root->value[0] == '='){

        char* left_type = get_type(root->left , Current_Scope);
        char* right_type = get_type(root->right , Current_Scope);

        printf("\nleft type : %s.\n",left_type);
        printf(" right type : %s.\n",right_type);

        if(strcmp(left_type , right_type) == 0){
            printf("\nType check passed.");
        }
        else{
            printf("\nType check ERROR : can not assign '%s' to '%s' variable.",right_type,left_type);
        }
    }
}

int is_declared(const char* line){
    return (strncmp(line , "char " , 5) == 0 || strncmp(line , "int " , 4) == 0 || strncmp(line , "float " , 6) == 0 || strncmp(line , "double " , 7) == 0);
}

void parse_declaration(const char* line){
    char type[10] , name[50];
    sscanf(line , "%s %s",type , name);

    if(name[strlen(name)-1] == ';'){
        name[strlen(name)-1] = '\0';
    }
    add_symbol(name , type);
}

void print_sym(){
    printf("\n----SYMBOL TABLE----\n");
    printf("%-15s %-10s %-15s\n","NAME","TYPE","INITIALIZED");
    for(int i=0 ; i<sym_count ; i++){
        printf("%-15s %-10s %-15s\n",sym_table[i].sym , sym_table[i].type , sym_table[i].is_initialized?"YES":"NO");
    }
}

void free_tree(NODE* root){
    if(root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
    
}

/*void semantic(const char* exp[] , NODE* root , const int* n){
    printf("Program started\n");
    int exp_count = *n;

    //char *line = exp;

    for(int i=0 ; i<exp_count ; i++){
        if(is_declared(exp[i])){
            parse_declaration(exp[i]);
        }
    }
    printf("\n");

    //NODE* root = parser(exp);
    //printf("it's here:\n");

    Check_Undeclared(root);
    Type_check(root);

    printf("it's ready\n");
    print_sym();

}*/