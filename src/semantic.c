#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"parser.h"
#include"tokenizer.h"
#include"semantic.h"
#include"compiler_result.h"

static char *datatype[] = {"INT" , "FLOAT" , "DOUBLE" , "CHAR" , "VOID"};

int is_float(const char* s){
    for(int i=0 ; s[i] != '\0' ; i++){
        if(s[i] == '.') return 1;
    }
    return 0;
}

int get_slot(const SymbolTable *table , const char *name , const char *scope){
    int slot = 0;

    
    for(int i=0 ; i<table->sym_count ; i++){
        if(strcmp(table->table[i].scope , "global") == 0) continue;

        if(strcmp(table->table[i].scope , scope) == 0){
            if(strcmp(table->table[i].sym , name) == 0){
                return slot;
            }
            slot++;
        }
    }
    return -1;
}

void add_symbol(Semantic_ctxt *context, const char* name , DataType type , int is_param , int size){
    for(int i=0 ; i<context->symbols.sym_count ; i++){
        if(strcmp(context->symbols.table[i].sym , name) == 0 && strcmp(context->symbols.table[i].scope , context->current_scope) == 0){
            printf("ERROR : The %s is already declared.\n",name);
            return;
        }
    }

    strcpy(context->symbols.table[context->symbols.sym_count].sym , name);
    context->symbols.table[context->symbols.sym_count].type = type;
    strcpy(context->symbols.table[context->symbols.sym_count].scope , context->current_scope);
    context->symbols.table[context->symbols.sym_count].is_initialized = 0;
    context->symbols.table[context->symbols.sym_count].is_param = is_param;
    context->symbols.table[context->symbols.sym_count].size = size;

    //printf("STORING : name = %s and is_param = %d\n", name , context->table.table[sym_count].is_param);
    context->symbols.sym_count++;
}

void Check_Undeclared(NODE* root , Semantic_ctxt *context){
    if(root == NULL){
        return;
    }

    if(root->type == AST_FUNCTION_CALL){
        for(int i=0 ; i<root->ARG_count ; i++){
            if(root->ARG[i]->type == AST_STRING) continue;
            Check_Undeclared(root->ARG[i] , context);
        }
        return;
    }

    

    if(isalpha(root->lexeme[0]) || root->lexeme[0] == '_'){

        if(strcmp(root->lexeme , "RETVAL") == 0){

        }

        else if(root->type == AST_CHARACTER){
            
        }
        else{
            int found = 0;
            for(int i=0 ; i<context->symbols.sym_count ; i++){
                if(strcmp(context->symbols.table[i].sym , root->lexeme) == 0 && strcmp(context->symbols.table[i].scope , context->current_scope) == 0){
                    found = 1;
                    break;
                }
            }

            if(!found){
                char base[50];

                strcpy(base , root->lexeme);
                //printf("BASE : %s\n",base);
                int len = strlen(base);
                int j = len-1;
                while(j>0 && isdigit(base[j-1])){
                    base[--j] = '\0';
                }

                //printf("BASE AFTER TRIMMING: %s\n",base);
                for(int i=0 ; i<context->symbols.sym_count ; i++){
                    if(strcmp(context->symbols.table[i].sym , base) == 0 && strcmp(context->symbols.table[i].scope , context->current_scope) == 0){
                        found = 1;
                        break;
                    }
                }
            }
            if(!found){
                printf("ERROR , %s is used , but not declared.\n",root->lexeme);
            }
        }
        
    }

    Check_Undeclared(root->left , context);
    Check_Undeclared(root->right , context);
}

DataType get_type(NODE* node , Semantic_ctxt *context){
    if(node == NULL) return TYPE_VOID;

    //printf("CHECKING NODE : %s.\n",node->lexeme);

    if(node->type == AST_FUNCTION_CALL){

        for(int i=0 ; i<context->symbols.sym_count ; i++){
            if(strcmp(node->lexeme , context->symbols.table[i].sym) == 0 && strcmp(context->symbols.table[i].scope , "global") == 0){
                //printf("the type is : %s.\n",context->table.table[i].type);
                return context->symbols.table[i].type;
            }
            
        }
        return TYPE_VOID;
    }

    char base[50];
        
    //printf("COPYING THE NODE'S VALUE TO THE BASE.\n");
    strcpy(base , node->lexeme);

    //printf("BASE : %s.\n",base);

    int len = strlen(base);

    //printf("LENGTH BEFORE:%d.\n",len);

    int j = len-1;

    while(j>0 && isdigit(base[j-1])){
        base[--j] = '\0';
        //printf("LENGTH : %d.\n",j);
    }
    //printf("BASE AFTER REDUCTION : %s.\n",base);

    for(int i=0 ; i<context->symbols.sym_count ; i++){
        if(strcmp(context->symbols.table[i].sym , base) == 0  && strcmp(context->symbols.table[i].scope , context->current_scope) == 0){
            return context->symbols.table[i].type;
        }
    }
    //return "UNKNOWN";
    

    if(node->left == NULL && node->right == NULL){
        if(isdigit(node->lexeme[0])){
            if(is_float(node->lexeme)) return TYPE_FLOAT;
            else return TYPE_INT;
        }
        else if(node->type == AST_CHARACTER){
            return TYPE_CHAR;
        }

        for(int i=0 ; i<context->symbols.sym_count ; i++){
            if(strcmp(context->symbols.table[i].sym , node->lexeme) == 0 && strcmp(context->symbols.table[i].scope , context->current_scope) == 0){
                //printf("the type is : %s.\n",context->table.table[i].type);
                return context->symbols.table[i].type;
            }
        }
        return TYPE_VOID;
    }

    

    DataType left_type = get_type(node->left , context);
    DataType right_type = get_type(node->right , context);

    if(left_type == TYPE_FLOAT || right_type == TYPE_FLOAT){
        return TYPE_FLOAT;
    }
    return TYPE_INT;
}

void Type_check(NODE* root , Semantic_ctxt *context){
    if(root == NULL) return ;

    if(root->lexeme[0] == '='){

        if(root->right != NULL && root->right->type == AST_FUNCTION_CALL){
            //printf("Type check passed(function call).\n");
            return;
        }
        
        //printf("Type check called with current scope %s\n",context->current_scope);
        //printf("root->left->lexeme = %s\n", root->left->lexeme);
        //printf("root->right->lexeme = %s\n", root->right->lexeme);  
        DataType left_type = get_type(root->left , context);
        DataType right_type = get_type(root->right , context);

        //printf("\nleft type : %s.\n",left_type);
        //printf(" right type : %s.\n",right_type);

        if(left_type == right_type){
            //printf("\nType check passed.");
        }
        else if(left_type == TYPE_FLOAT && right_type == TYPE_INT){
            //printf("\nType check passed (implicit conversion of int->float).\n");
        }
        else if( right_type == TYPE_FLOAT && left_type == TYPE_INT ){
            //printf("Type check passed , (float to int implicit conversion , possible data loss!)");
        }
        else if((right_type == TYPE_DOUBLE && left_type == TYPE_FLOAT) || (right_type == TYPE_FLOAT && left_type == TYPE_DOUBLE)){

        }
        else{
            printf("\nType check ERROR : can not assign '%s' to '%s' variable.",datatype[right_type] , datatype[left_type]);
        }
    }
}

int is_declared(const char* line){
    return (strncmp(line , "char " , 5) == 0 || strncmp(line , "int " , 4) == 0 || strncmp(line , "float " , 6) == 0 || strncmp(line , "double " , 7) == 0);
}

DataType stringtotype(const char type[20]){
    if(strcmp(type , "int") == 0){
        return TYPE_INT;
    }
    else if(strcmp(type , "float") == 0){
        return TYPE_FLOAT;
    }
    else if(strcmp(type , "char") == 0){
        return TYPE_CHAR;
    }
    else if(strcmp(type , "double") == 0){
        return TYPE_DOUBLE;
    }
    else{
        return TYPE_VOID;
    }
}

void parse_declaration(const char* line , Semantic_ctxt *context){
    DataType type ;
    char type_string[20];
    char name[50];
    sscanf(line , "%s %s", type_string , name);

    if(name[strlen(name)-1] == ';'){
        name[strlen(name)-1] = '\0';
    }

    type = stringtotype(type_string);
    add_symbol(context , name , type , 0 , 0);
}

void append_symbol(char *buffer , char *line){
    strcat(buffer , line);
}

void BUILD_SYMBOL_TEXT(const SymbolTable *table , char *buffer){
    buffer[0] = '\0';
    char line[256];
    sprintf(line , "%7s","\n----SYMBOL TABLE----\n");
    append_symbol(buffer , line);
    sprintf(line , "\n");
    append_symbol(buffer , line);

    sprintf(line , "%-20s %-10s %-15s %-15s %-10s %-10s\n","NAME","TYPE","INITIALIZED","SCOPE","IS_PARAM" , "SIZE");

    append_symbol(buffer , line);

    for(int i=0 ; i<table->sym_count ; i++){
        sprintf(line , "%-20s %-10s %-15s %-15s %-10s %-10d\n",table->table[i].sym , datatype[table->table[i].type] , table->table[i].is_initialized?"YES":"NO" , table->table[i].scope , table->table[i].is_param? "YES":"NO" , table->table[i].size);
        append_symbol(buffer , line);
    }
}

const char *Symbol_Text(char *buffer){
    return buffer;
}

void free_tree(NODE* root){
    if(root == NULL) return;
    for(int i=0 ; i<root->ARG_count ; i++){
        free_tree(root->ARG[i]);
    }
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