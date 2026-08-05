#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"tokenizer.h"
#include"parser.h"

#define ARRAY_SIZE 50

NODE* create_node(const char *lexeme , AST_NODE_TYPE type){
    //printf("node creation : '%s'.\n",exp);
    NODE* new = (NODE*)malloc(sizeof(NODE));

    if(new == NULL){
        fprintf(stderr , "MEMORY ALLOCATION FAILED.\n");
        exit(EXIT_FAILURE);
    }
    //printf("inserting lexeme.\n");
    strncpy(new->lexeme , lexeme , LEXEME_SIZE-1);
    new->lexeme[LEXEME_SIZE-1] = '\0';
    //printf("the new node contain %s .\n",new->lexeme);
    new->left = NULL;
    new->right = NULL;
    new->type = type;
    new->ARG_count = 0;
    for(int i=0 ; i<MAX_ARGUMENT ; ){
        new->ARG[i] = NULL;
    }
    return new;
}

void print_AST(NODE* root){
    if(root == NULL){
        return;
    }
    //printf("Inorder traversal of the syntax tree:\n");
    print_AST(root->left);
    printf(" %s ",root->lexeme);
    print_AST(root->right);
    //printf("\n syntax tree printed.\n");
}

int find_operator(const TOKEN tokens[] , int start , int end){
    int depth = 0;

    for(int i=end ; i>=start ; i--){
        if (strcmp(tokens[i].lexeme , ")")==0)
        {
            depth++;
        }
        else if(strcmp(tokens[i].lexeme , "(")==0){
            depth--;
        }
        else if(depth == 0 && tokens[i].tokentype == TOKEN_OPERATOR && strcmp(tokens[i].lexeme , "=")==0){
            return i;
        }
    }

    depth = 0; 

    for(int i=end ; i>=start ; i--){
        if (strcmp(tokens[i].lexeme , ")")==0)
        {
            depth++;
        }
        else if(strcmp(tokens[i].lexeme ,"(")==0){
            depth--;
        }
        else if(depth == 0 && tokens[i].tokentype == TOKEN_OPERATOR && (strcmp(tokens[i].lexeme , "+") == 0 || strcmp(tokens[i].lexeme , "-") == 0)){
            return i;
        }
    }

    depth = 0; 

    for(int i=end ; i>=start ; i--){
        if (strcmp(tokens[i].lexeme , ")")==0)
        {
            depth++;
        }
        else if(strcmp(tokens[i].lexeme , "(")==0){
            depth--;
        }
        else if(depth == 0 && tokens[i].tokentype == TOKEN_OPERATOR && (strcmp(tokens[i].lexeme , "*")==0 || strcmp(tokens[i].lexeme , "/")==0 || strcmp(tokens[i].lexeme , "%")==0)){
            return i;
        }
    }
    return -1;
}

NODE* build_AST(const TOKEN tokens[] , int start , int end){
    //printf("Building AST for tokens from %d to %d.\n",start , end);
    //printf("the tokens are %s , %s\n",tokens[start].lexeme , tokens[end].lexeme);
    if(start == end){
        if(tokens[start].tokentype == TOKEN_CHARACTER){
            NODE* char_node = create_node(tokens[start].lexeme , AST_CHARACTER);
            return char_node;
        }
        switch(tokens[start].tokentype){
            case TOKEN_IDENTIFIER:
                return create_node(tokens[start].lexeme , AST_IDENTIFIER);
            
            case TOKEN_CONSTANT:
                return create_node(tokens[start].lexeme , AST_NUMBER);
            
            case TOKEN_OPERATOR:
                return create_node(tokens[start].lexeme , AST_OPERATOR);

            case TOKEN_FUNCTION:
                return create_node(tokens[start].lexeme , AST_FUNCTION_CALL);

            default:
                return NULL;
        }
    }

    if(tokens[start].tokentype ==TOKEN_IDENTIFIER && (strcmp(tokens[start+1].lexeme , "[") == 0)){
        //printf("the tokens are %s at %d , %s at %d , %s at %d.\n",tokens[start].lexeme , start , tokens[start+1].lexeme , start+1 , tokens[start+2].lexeme , start+2);
        char arr_name[ARRAY_SIZE];
        
        sprintf(arr_name ,"%s%s " ,tokens[start].lexeme , tokens[start+2].lexeme);

        if(start+4 <= end && strcmp(tokens[start+4].lexeme , "=") == 0){
            NODE* arr_node = create_node(arr_name , AST_IDENTIFIER);

            NODE* root = create_node(tokens[start+4].lexeme , AST_OPERATOR);

            root->left = arr_node;
            root->right = build_AST(tokens , start+5 , end);

            return root;
        }
        else{
            return create_node(arr_name , AST_IDENTIFIER);
        }
        //printf("array name : %s\n",arr_name);
    }

    if(strcmp(tokens[start].lexeme , "&") == 0){
        //printf("address of operator detected for %s\n",tokens[start+1].lexeme);
        NODE* addr_node = create_node(tokens[start].lexeme , AST_ADDRESS_OF);
        //NODE* left = create_node(tokens[start+1].lexeme);
        //addr_node->left = left;
        addr_node->right = build_AST(tokens , start+1 , end);
        return addr_node;
    }

    if(strcmp(tokens[start].lexeme , "*") == 0 && tokens[start+1].tokentype ==TOKEN_IDENTIFIER ){
        if(start + 2 < end && strcmp(tokens[start+2].lexeme , "=") == 0){
            //printf("creating node for dereferencing.\n");
            //printf("start = %d and end = %d.\n",start , end);
            //printf("tokens[start+1].lexeme = %s , tokens[start+2].lexeme = %s , tokens[start+3].lexeme = %s\n",tokens[start+1].lexeme , tokens[start+2].lexeme , tokens[start+3].lexeme);
            NODE* deref_node = create_node(tokens[start+2].lexeme , AST_OPERATOR);
            //printf("creating left node.\n");
            deref_node->left = build_AST(tokens , start , start+1);
            //NODE* left_l_node = create_node(tokens[start+1].lexeme);
            //left_node->left = left_l_node;
            //printf("creating right node");
            
            deref_node->right = (start + 3 <= end )? build_AST(tokens , start+3 , end) : NULL;
            //printf("dereference node = %s\n",deref_node->lexeme);
            return deref_node;
        }
        else{
            NODE* deref_node = create_node(tokens[start].lexeme , AST_DEREFERENCE);
            deref_node->right = build_AST(tokens , start+1 , end);
            return deref_node;
        }
        
    }

    if(tokens[start].tokentype == TOKEN_FUNCTION){
        NODE* call_node = create_node(tokens[start].lexeme , AST_FUNCTION_CALL);
        call_node->ARG_count = 0;
        
        int arg_pos = start + 2;

        while(strcmp(tokens[arg_pos].lexeme , ")") != 0){
            int arg_end = arg_pos;
            //printf("tokens[%d].lexeme = %s\n",arg_end , tokens[arg_end].lexeme);
            if(tokens[arg_end].tokentype == TOKEN_STRING){
                NODE *arg_node = create_node(tokens[arg_end].lexeme , AST_STRING);
                call_node->ARG[call_node->ARG_count++] = arg_node;
                //printf("ARG[%d] = %s\n",arg_end , arg_node->lexeme);
                arg_end++;
            }
            else{
                while(strcmp(tokens[arg_end].lexeme , ",") != 0 && strcmp(tokens[arg_end].lexeme , ")") != 0){
                arg_end++;
                }

                call_node->ARG[call_node->ARG_count++] = build_AST(tokens , arg_pos , arg_end-1);
            }
            
            //printf("arg_end = %d\n",arg_end);
            //printf("tokens[%d] = %s\n",arg_end , tokens[arg_end].lexeme);
            if(strcmp(tokens[arg_end].lexeme , ",") == 0){
                arg_pos = arg_end + 1;
                //printf("tokens[%d].lexeme = %s\n",arg_pos , tokens[arg_pos].lexeme);
            }
            else{
                break;
            }
        }
        //printf("BUILDING CALL NODE FOR %s\n",tokens[start].lexeme);

        //printf("total argument found : %d\n",call_node->ARG_count);

        //for(int i=0 ; i<call_node->ARG_count ; ){
        //    printf("ARG[%d] = %s , is_string = %d\n",i , call_node->ARG[i] , call_node->ARG[i]->is_string);
        //}
        return call_node;
    }

    int is_wrapped = 1;
    
    if(strcmp(tokens[start].lexeme , "(") != 0 && strcmp(tokens[end].lexeme , ")") != 0){
        is_wrapped = 0;
    }
    else{
        int depth = 0;
        for(int i=start ; i<=end ; ){
            if(strcmp(tokens[i].lexeme , "(")==0){
            depth++;
            }
            else if(strcmp(tokens[i].lexeme , ")")==0){
            depth--;
            }
            if(depth == 0 && i<end){
            is_wrapped = 0;
            break;
            } 
        }
    }
    

    if(is_wrapped){
        return build_AST(tokens , start+1 , end-1);
    }
    //if(start<=end && tokens[start].lexeme == '(' && tokens[end].lexeme == ')'){
      //  return Build_AST(tokens , start+1 , end-1);
    //}

    int pos = find_operator(tokens , start , end);

    if(pos == -1){
        return create_node(tokens[start].lexeme , AST_OPERATOR);
    }

    NODE* root = create_node(tokens[pos].lexeme , AST_OPERATOR);
    //printf("the operator is %s at position %d\n",tokens[pos].lexeme , pos);
    root->left = build_AST(tokens , start , pos-1);
    root->right = build_AST(tokens , pos+1 , end);

    return root;
}
