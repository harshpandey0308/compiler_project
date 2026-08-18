#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"tokenizer.h"
#include"parser.h"

#define ARRAY_SIZE 50

NODE* create_node(const char *lexeme , AST_NODE_TYPE type){
    //printf("node creation : '%s'.\n",lexeme);
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
    //printf("left = null.\n");
    new->right = NULL;
    //printf("right = null.\n");
    new->next = NULL;
    new->type = type;
    new->for_node = NULL;
    new->body = NULL;
    new->cond = NULL;
    //printf("type = %d\n",type);
    new->ARG_count = 0;
    //printf("initializing argument.\n");
    for(int i=0 ; i<MAX_ARGUMENT ; i++){
        //printf("i = %d.\n",i);
        new->ARG[i] = NULL;
    }
    //printf("node creation completed.\n");
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

int find_operator(const TokenEntry *token_table , int start , int end){
    printf("the tokens start = %d and end = %d.\n",start , end);
    int depth = 0;

    for(int i=end ; i>=start ; i--){
        if (strcmp(token_table->tokens[i].lexeme , ")")==0)
        {
            depth++;
        }
        else if(strcmp(token_table->tokens[i].lexeme , "(")==0){
            depth--;
        }
        else if(depth == 0 && token_table->tokens[i].tokentype == TOKEN_OPERATOR && strcmp(token_table->tokens[i].lexeme , "=")==0){
            return i;
        }
    }

    depth = 0; 

    for(int i=end ; i>=start ; i--){
        if (strcmp(token_table->tokens[i].lexeme , ")")==0)
        {
            depth++;
        }
        else if(strcmp(token_table->tokens[i].lexeme ,"(")==0){
            depth--;
        }
        else if(depth == 0 && token_table->tokens[i].tokentype == TOKEN_OPERATOR && (strcmp(token_table->tokens[i].lexeme , "+") == 0 || strcmp(token_table->tokens[i].lexeme , "-") == 0)){
            return i;
        }
    }

    depth = 0; 

    for(int i=end ; i>=start ; i--){
        if (strcmp(token_table->tokens[i].lexeme , ")")==0)
        {
            depth++;
        }
        else if(strcmp(token_table->tokens[i].lexeme , "(")==0){
            depth--;
        }
        else if(depth == 0 && token_table->tokens[i].tokentype == TOKEN_OPERATOR && (strcmp(token_table->tokens[i].lexeme , "*")==0 || strcmp(token_table->tokens[i].lexeme , "/")==0 || strcmp(token_table->tokens[i].lexeme , "%")==0)){
            printf("i=%d.\n",i);
            return i;
        }
    }

    depth = 0;

    for(int i=end ; i>=start ; i--){
        if(strcmp(token_table->tokens[i].lexeme , ")") == 0){
            depth++;
        }
        else if(strcmp(token_table->tokens[i].lexeme , "(") == 0){
            depth--;
        }
        else if(depth == 0 && token_table->tokens[i].tokentype == TOKEN_COMPARATOR && (strcmp(token_table->tokens[i].lexeme , "<") == 0 || strcmp(token_table->tokens[i].lexeme , "<=") == 0)){
            printf("i = %d.\n",i);
            return i;
        }
    }

    depth = 0;

    for(int i=end ; i>=start ; i--){
        if(strcmp(token_table->tokens[i].lexeme , ")") == 0){
            depth++;
        }
        else if(strcmp(token_table->tokens[i].lexeme , "(") == 0){
            depth--;
        }
        else if(depth == 0 && token_table->tokens[i].tokentype == TOKEN_COMPARATOR && (strcmp(token_table->tokens[i].lexeme , ">") == 0 || strcmp(token_table->tokens[i].lexeme , ">=") == 0)){
            printf("i = %d.\n",i);
            return i;
        }
    }

    depth = 0;

    for(int i=end ; i>=start ; i--){
        if(strcmp(token_table->tokens[i].lexeme , ")") == 0){
            depth++;
        }
        else if(strcmp(token_table->tokens[i].lexeme , "(") == 0){
            depth--;
        }
        else if(depth == 0 && token_table->tokens[i].tokentype == TOKEN_COMPARATOR && (strcmp(token_table->tokens[i].lexeme , "!=") == 0 || strcmp(token_table->tokens[i].lexeme , "==") == 0)){
            printf("i = %d.\n",i);
            return i;
        }
    }

    return -1;
}

NODE* build_AST(const TokenEntry *token_table , int start , int end){

    if(start > end){
        return NULL;
    }
    printf("Building AST for token_table->tokens from %d to %d.\n",start , end);
    //printf("\n");
    printf("the token_table->tokens are %s , %s , %s\n",token_table->tokens[start].lexeme , token_table->tokens[start+1].lexeme ,token_table->tokens[end].lexeme);
    if(start == end){
        printf("the token is %s.\n",token_table->tokens[start].lexeme);
        if(token_table->tokens[start].tokentype == TOKEN_CHARACTER){
            NODE* char_node = create_node(token_table->tokens[start].lexeme , AST_CHARACTER);
            return char_node;
        }
        switch(token_table->tokens[start].tokentype){
            case TOKEN_IDENTIFIER:
                printf("the identifier is %s.\n",token_table->tokens[start].lexeme);
                return create_node(token_table->tokens[start].lexeme , AST_IDENTIFIER);
            
            case TOKEN_CONSTANT:
                return create_node(token_table->tokens[start].lexeme , AST_NUMBER);
            
            case TOKEN_OPERATOR:
                return create_node(token_table->tokens[start].lexeme , AST_OPERATOR);

            case TOKEN_FUNCTION:
                return create_node(token_table->tokens[start].lexeme , AST_FUNCTION_CALL);

            default:
                return NULL;
        }
    }

    if(strcmp(token_table->tokens[start].lexeme , "if") == 0 || strcmp(token_table->tokens[start].lexeme , "else") == 0){
        NODE *cond_statement_node = parse_cond(token_table , &start);
        return cond_statement_node;
    }

    if(strcmp(token_table->tokens[start].lexeme , "while") == 0 || strcmp(token_table->tokens[start].lexeme , "for") == 0){
        NODE *loop_root = parse_loop(token_table , &start);
        return loop_root;
    }

    //printf("the token is at identifier and array checking.\n");
    if(token_table->tokens[start].tokentype ==TOKEN_IDENTIFIER && (strcmp(token_table->tokens[start+1].lexeme , "[") == 0)){
        printf("the token_table->tokens are %s at %d , %s at %d , %s at %d.\n",token_table->tokens[start].lexeme , start , token_table->tokens[start+1].lexeme , start+1 , token_table->tokens[start+2].lexeme , start+2);
        char arr_name[ARRAY_SIZE];
        
        sprintf(arr_name ,"%s%s " ,token_table->tokens[start].lexeme , token_table->tokens[start+2].lexeme);

        if(start+4 <= end && strcmp(token_table->tokens[start+4].lexeme , "=") == 0){
            NODE* arr_node = create_node(arr_name , AST_IDENTIFIER);

            NODE* root1 = create_node(token_table->tokens[start+4].lexeme , AST_OPERATOR);

            root1->left = arr_node;
            root1->right = build_AST(token_table , start+5 , end);

            return root1;
        }
        //else{
            //return create_node(arr_name , AST_IDENTIFIER);
        //}
        //printf("array name : %s\n",arr_name);
    }

    if(strcmp(token_table->tokens[start].lexeme , "&") == 0){
        //printf("address of operator detected for %s\n",token_table->tokens[start+1].lexeme);
        NODE* addr_node = create_node(token_table->tokens[start].lexeme , AST_ADDRESS_OF);
        //NODE* left = create_node(token_table->tokens[start+1].lexeme);
        //addr_node->left = left;
        addr_node->right = build_AST(token_table , start+1 , end);
        return addr_node;
    }

    if(strcmp(token_table->tokens[start].lexeme , "*") == 0 && token_table->tokens[start+1].tokentype ==TOKEN_IDENTIFIER ){
        if(start + 2 < end && strcmp(token_table->tokens[start+2].lexeme , "=") == 0){
            //printf("creating node for dereferencing.\n");
            //printf("start = %d and end = %d.\n",start , end);
            //printf("token_table->tokens[start+1].lexeme = %s , token_table->tokens[start+2].lexeme = %s , token_table->tokens[start+3].lexeme = %s\n",token_table->tokens[start+1].lexeme , token_table->tokens[start+2].lexeme , token_table->tokens[start+3].lexeme);
            NODE* deref_node = create_node(token_table->tokens[start+2].lexeme , AST_OPERATOR);
            //printf("creating left node.\n");
            deref_node->left = build_AST(token_table , start , start+1);
            //NODE* left_l_node = create_node(token_table->tokens[start+1].lexeme);
            //left_node->left = left_l_node;
            //printf("creating right node");
            
            deref_node->right = (start + 3 <= end )? build_AST(token_table , start+3 , end) : NULL;
            //printf("dereference node = %s\n",deref_node->lexeme);
            return deref_node;
        }
        else{
            NODE* deref_node = create_node(token_table->tokens[start].lexeme , AST_DEREFERENCE);
            deref_node->right = build_AST(token_table , start+1 , end);
            return deref_node;
        }
        
    }

    if(strcmp(token_table->tokens[start+1].lexeme , "++") == 0){
        NODE *root_ = create_node(token_table->tokens[start+1].lexeme , AST_INCREMENT);
        root_->right = build_AST(token_table , start , start);
        return root_;
    }
    
    

    if(token_table->tokens[start].tokentype == TOKEN_FUNCTION){
        //printf("the token is funtion.\n");
        NODE* call_node = create_node(token_table->tokens[start].lexeme , AST_FUNCTION_CALL);
        if(call_node == NULL){
            return NULL;
        }
        call_node->ARG_count = 0;
        
        int arg_pos = start+2;
        //printf("arg_pos = %d and tokens[%d].lexeme = %s\n",arg_pos , arg_pos , token_table->tokens[arg_pos].lexeme);

        while(strcmp(token_table->tokens[arg_pos].lexeme , ")") != 0){
            int arg_end = arg_pos;
            //printf("token_table->tokens[%d].lexeme = %s\n",arg_end , token_table->tokens[arg_end].lexeme);
            if(token_table->tokens[arg_end].tokentype == TOKEN_STRING){
                NODE *arg_node = create_node(token_table->tokens[arg_end].lexeme , AST_STRING);
                call_node->ARG[call_node->ARG_count++] = arg_node;
                //printf("ARG[%d] = %s\n",arg_end , arg_node->lexeme);
                arg_end++;
            }
            else{
                while(strcmp(token_table->tokens[arg_end].lexeme , ",") != 0 && strcmp(token_table->tokens[arg_end].lexeme , ")") != 0){
                arg_end++;
                }

                call_node->ARG[call_node->ARG_count++] = build_AST(token_table , arg_pos , arg_end-1);
            }
            
            //printf("arg_end = %d\n",arg_end);
            //printf("token_table->tokens[%d] = %s\n",arg_end , token_table->tokens[arg_end].lexeme);
            if(strcmp(token_table->tokens[arg_end].lexeme , ",") == 0){
                arg_pos = arg_end + 1;
                //printf("token_table->tokens[%d].lexeme = %s\n",arg_pos , token_table->tokens[arg_pos].lexeme);
            }
            else{
                break;
            }
        }
        //printf("BUILDING CALL NODE FOR %s\n",token_table->tokens[start].lexeme);

        //printf("total argument found : %d\n",call_node->ARG_count);

        //for(int i=0 ; i<call_node->ARG_count ; ){
        //    printf("ARG[%d] = %s , is_string = %d\n",i , call_node->ARG[i] , call_node->ARG[i]->is_string);
        //}
        return call_node;
    }

    int is_wrapped = 1;
    
    if(strcmp(token_table->tokens[start].lexeme , "(") != 0 && strcmp(token_table->tokens[end].lexeme , ")") != 0){
        is_wrapped = 0;
    }
    else{
        int depth = 0;
        for(int i=start ; i<=end ; i++){
            if(strcmp(token_table->tokens[i].lexeme , "(")==0){
            depth++;
            }
            else if(strcmp(token_table->tokens[i].lexeme , ")")==0){
            depth--;
            }
            if(depth == 0 && i<end){
            is_wrapped = 0;
            break;
            } 
        }
    }
    

    if(is_wrapped){
        printf("the expression is wrapped.\n");
        return build_AST(token_table , start+1 , end-1);
    }
    //if(start<=end && token_table->tokens[start].lexeme == '(' && token_table->tokens[end].lexeme == ')'){
      //  return Build_AST(token_table->tokens , start+1 , end-1);
    //}

    int pos = find_operator(token_table , start , end);
    printf("position = %d.\n",pos);

    if(pos == -1){
        return create_node(token_table->tokens[start].lexeme , AST_OPERATOR);
    }

    NODE* root = create_node(token_table->tokens[pos].lexeme , AST_OPERATOR);
    printf("the operator is %s at position %d\n",token_table->tokens[pos].lexeme , pos);
    root->left = build_AST(token_table , start , pos-1);
    root->right = build_AST(token_table , pos+1 , end);

    printf("the left child is %s.\n",root->left->lexeme);
    printf("the right child is %s.\n",root->right->lexeme);

    return root;
}

NODE *parse_cond(const TokenEntry *token_table , int *start){
    BODY *body_node = malloc(sizeof(BODY));
    
    body_node->head = NULL;
    body_node->tail = NULL;

    if(strcmp(token_table->tokens[*start].lexeme , "if") == 0){
        NODE *root = create_node(token_table->tokens[*start].lexeme , AST_IF);

        printf("the root is %s.\n",root->lexeme);
        printf("the start is %d.\n",*start);

        int condition_start = *(start) + 2;
        int condition_end = condition_start;
        

        int if_cond_depth = 1;
        while(condition_end < token_table->token_count && if_cond_depth != 0){
            printf("lexeme = %s at %d.\n",token_table->tokens[condition_end].lexeme , condition_end);
            if(strcmp(token_table->tokens[condition_end].lexeme , "(") == 0){
                if_cond_depth++;
            }
            else if(strcmp(token_table->tokens[condition_end].lexeme , ")") == 0){
                if_cond_depth--;
            }
            condition_end++;
            printf("condition depth = %d.\n",if_cond_depth);
        }

        printf("condition end at %d.\n",condition_end);
        NODE *cond_node = build_AST(token_table , condition_start , condition_end-2);

        root->cond = cond_node;

        printf("cond node of if : %s.\n",cond_node->lexeme);
        printf("cond node left = %s.\n",cond_node->left->lexeme);
        printf("right node = %s.\n",cond_node->right->lexeme);
        
        int body_begin = condition_end+1;
        int body_end = body_begin;

        int depth = 1;

        while(body_end < token_table->token_count && depth != 0){
            if(strcmp(token_table->tokens[body_end].lexeme , "{") == 0){
                depth++;
            }
            else if(strcmp(token_table->tokens[body_end].lexeme , "}") == 0){
                depth--;
            }
            body_end++;
        }

        printf("the body_end is = %d.\n",body_end);
        
        int j = body_begin;
        int *k = &j;

        NODE *temp = NULL;

        while(*k < token_table->token_count && *k < body_end-1){
            if(strcmp(token_table->tokens[*k].lexeme , "while") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "for") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "if") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_cond(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_cond(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(token_table->tokens[*k].tokentype == TOKEN_FUNCTION){
                int end = *k;
                while(end < token_table->token_count && strcmp(token_table->tokens[end].lexeme , ";") != 0){
                    end++;
                }
                temp = build_AST(token_table , *k , end-1);
                if(body_node->head == NULL){
                    body_node->head = temp;
                    body_node->tail = temp;
                }
                else{
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
                *k = end + 1;
            }
            else{
                int end = *k;
                while(strcmp(token_table->tokens[end].lexeme , ";") != 0){
                    end++;
                }
                if(body_node->head == NULL){
                    body_node->head = build_AST(token_table , *k , end-1);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = build_AST(token_table , *k , end-1);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
        }
        
        root->body = body_node;

        printf("root left child : %s.\n",root->cond->lexeme);
        printf("root body = %s.\n",root->body->head->lexeme);

        *start = body_end;

        return root;
    }
    else if(strcmp(token_table->tokens[*start].lexeme , "else") == 0 && strcmp(token_table->tokens[*start+1].lexeme , "(") == 0){
        
        NODE *root = create_node(token_table->tokens[*start].lexeme , AST_ELSE);

        int body_start = *start + 2;
        int body_end = body_start;

        int depth =1;
        while(body_end < token_table->token_count && depth != 0){
            if(strcmp(token_table->tokens[body_end].lexeme , "{") == 0){
                depth++;
            }
            else if(strcmp(token_table->tokens[body_end].lexeme , "}") == 0){
                depth--;
            }

            body_end++;
        }

        int s = body_start;
        int *k = &s;
        NODE *temp = NULL;

        while(*k < token_table->token_count && *k < body_end-1){
            if(strcmp(token_table->tokens[*k].lexeme , "while") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "for") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "if") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_cond(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_cond(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else{
                int end = *k;
                while(end < token_table->token_count && strcmp(token_table->tokens[end].lexeme , ";") != 0){
                    end++;
                }

                if(body_node->head == NULL){
                    body_node->head = build_AST(token_table , *k , end-1);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = build_AST(token_table , *k , end-1);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                    
                }
                *k = end+1;
            }
        }

        root->body = body_node;

        *start = body_end;

        return root;
    }

    else if(strcmp(token_table->tokens[*start].lexeme , "else") == 0 && strcmp(token_table->tokens[*start+1].lexeme , "if") == 0){
        NODE *root = create_node(token_table->tokens[*start].lexeme , AST_ELSEIF);

        int condition_start = *(start) + 3;
        int condition_end = *(start);
        
        int conditional_depth = 1;
        while(condition_end < token_table->token_count && conditional_depth != 0){
            if(strcmp(token_table->tokens[condition_end].lexeme , "(") == 0){
                conditional_depth++;
            }
            else if(strcmp(token_table->tokens[condition_end].lexeme , ")") == 0){
                conditional_depth--;
            }
            condition_end++;
        }

        NODE *cond_node = build_AST(token_table , condition_start , condition_end-1);

        root->cond = cond_node;
        
        int body_begin = condition_end+2;
        int body_end = body_begin;

        int depth = 1;
        while(body_end < token_table->token_count && depth != 0){
            if(strcmp(token_table->tokens[body_end].lexeme , "{") == 0){
                depth++;
            }
            else if(strcmp(token_table->tokens[body_end].lexeme , "}") == 0){
                depth--;
            }
            body_end++;
        }

        printf("the body_end is = %d.\n",body_end);
        int j = body_begin;
        int *k = &j;
        NODE *temp = NULL;

        while(*k < token_table->token_count && *k < body_end-1){
            if(strcmp(token_table->tokens[*k].lexeme , "while") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "for") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;

                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "if") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_cond(token_table , k);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = parse_cond(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else{
                int end = *k;
                while(strcmp(token_table->tokens[end].lexeme , ";") != 0){
                    end++;
                }
                if(body_node->head == NULL){
                    body_node->head = build_AST(token_table , *k , end-1);
                    body_node->tail = body_node->head;
                    body_node->tail->next = NULL;
                }
                else{
                    temp = build_AST(token_table , *k , end-1);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
                *k = end+1;
            }
        }
        root->body = body_node;

        *start = body_end;

        return root;
    }
    return NULL;
}


NODE *parse_loop(const TokenEntry *token_table , int *start){
    BODY *body_node = malloc(sizeof(BODY));

    body_node->head = NULL;
    body_node->tail = NULL;

    if(strcmp(token_table->tokens[*start].lexeme , "while") == 0){
        NODE *root = create_node(token_table->tokens[*start].lexeme , AST_WHILE);

        printf("while node check : root = %s.\n",root->lexeme);

        int condition_start = (*start)+2;
        int condition_end = condition_start;

        int cond_depth = 1;
        while(condition_end < token_table->token_count && cond_depth != 0){
            if(strcmp(token_table->tokens[condition_end].lexeme , "(") == 0){
                cond_depth++;
            }
            else if(strcmp(token_table->tokens[condition_end].lexeme , ")") == 0){
                cond_depth--;
            }
            condition_end++;
        }

        NODE *cond_node = build_AST(token_table , condition_start , condition_end-2);

        printf("cond node of while : %s.\n",cond_node->lexeme);
        printf("left child : %s.\n",cond_node->left->lexeme);
        printf("right child : %s.\n",cond_node->right->lexeme);

        root->cond = cond_node;

        int body_start = condition_end + 1;
        int body_end = body_start;

        int depth = 1;

        while(body_end < token_table->token_count && depth != 0){
            if(strcmp(token_table->tokens[body_end].lexeme , "{") == 0){
                depth++;
            }
            else if(strcmp(token_table->tokens[body_end].lexeme , "}") == 0){
                depth--;
            }

            body_end++;
        }

        int *k = &body_start;

        NODE *temp = NULL;
        while(*k < token_table->token_count && *k < body_end-1){

            if(strcmp(token_table->tokens[*k].lexeme , "while") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                }
                else{
                    temp = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "for") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , k);
                    body_node->tail = body_node->head;
                }
                else{
                    temp  = parse_loop(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*k].lexeme , "if") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_cond(token_table , k);
                    body_node->tail = body_node->head;
                }
                else{
                    temp = parse_cond(token_table , k);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else{
                int end = *k;
                while(end < token_table->token_count && strcmp(token_table->tokens[end].lexeme , ";") != 0){
                    end++;
                }
                if(body_node->head == NULL){
                    body_node->head = build_AST(token_table , *k , end-1);
                    body_node->tail = body_node->head;
                    *k = end+1;
                }
                else{
                    temp = build_AST(token_table , *k , end-1);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                    *k = end+1;
                }
            }
            
            
        }

        *start = body_end;

        root->body = body_node;

        printf("root left child : %s.\n",root->cond->lexeme);
        printf("root right child : %s.\n",root->body->head->lexeme);

        return root;
    }

    else if(strcmp(token_table->tokens[*start].lexeme , "for") == 0){
        FOR_NODE *for_node = malloc(sizeof(FOR_NODE));

        for_node->cond_node = NULL;
        for_node->init_node = NULL;
        for_node->update_node = NULL;
        
        NODE * root = create_node(token_table->tokens[*start].lexeme , AST_FOR);

        printf("the for loop starts at : %d.\n",*start);

        int header_start = *start+2;
        int header_end = header_start;

        int header_depth = 1;
        while(header_end < token_table->token_count &&  header_depth != 0){
            if(strcmp(token_table->tokens[header_end].lexeme , "(") == 0){
                header_depth++;
            }
            else if(strcmp(token_table->tokens[header_end].lexeme , ")") == 0){
                header_depth--;
            }
            header_end++;
        }

        int init_start = header_start;

        int init_end = init_start;

        while(init_end < token_table->token_count && strcmp(token_table->tokens[init_end].lexeme , ";") != 0){
            init_end++;
        }

        for_node->init_node = build_AST(token_table , init_start , init_end-1);
        printf("the initialization root = %s.\n",(for_node->init_node->lexeme));
        printf("the left child : %s.\n",(for_node->init_node->left->lexeme));
        printf("the right child is %s.\n",for_node->init_node->right->lexeme);

        int cond_start = init_end+1;
        int cond_end = cond_start;

        while(cond_end < token_table->token_count && strcmp(token_table->tokens[cond_end].lexeme , ";") != 0){
            cond_end++;
        }
        
        printf("conditional node starts at %d and end at %d.\n",cond_start , cond_end);
        for_node->cond_node = build_AST(token_table , cond_start , cond_end-1);

        printf("the conditional node = %s.\n", *(for_node->cond_node));
        printf("node -> left = %s and node -> right = %s.\n",for_node->cond_node->left->lexeme , for_node->cond_node->right->lexeme);

        int update_start = cond_end+1;
        int update_end = update_start;

        while(update_end < token_table->token_count && strcmp(token_table->tokens[update_end].lexeme , ")") != 0){
            update_end++;
        }

        for_node->update_node = build_AST(token_table , update_start , update_end-1);

        int body_start = update_end + 2;
        int body_end = body_start;
        
        int depth = 1;
        while(body_end < token_table->token_count && depth != 0){
            if(strcmp(token_table->tokens[body_end].lexeme , "{") == 0){
                depth++;
            }
            else if(strcmp(token_table->tokens[body_end].lexeme , "}") == 0){
                depth--;
            }
            body_end++;
        }

        int k = body_start;
        int *j = &k;
        
        NODE *temp = NULL;
        while(*j < token_table->token_count && *j < body_end-1){
            if(strcmp(token_table->tokens[*j].lexeme , "while") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , j);
                    body_node->tail = body_node->head;
                }
                else{
                    temp = parse_loop(token_table , j);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*j].lexeme , "for") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_loop(token_table , j);
                    body_node->tail = body_node->head;
                }
                else{
                    temp  = parse_loop(token_table , j);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else if(strcmp(token_table->tokens[*j].lexeme , "if") == 0){
                if(body_node->head == NULL){
                    body_node->head = parse_cond(token_table , j);
                    body_node->tail = body_node->head;
                }
                else{
                    temp = parse_cond(token_table , j);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                }
            }
            else{
                int end = *j;
                while(end < token_table->token_count && strcmp(token_table->tokens[end].lexeme , ";") != 0){
                    end++;
                }
                if(body_node->head == NULL){
                    body_node->head = build_AST(token_table , *j , end-1);
                    body_node->tail = body_node->head;
                    *j = end+1;
                }
                else{
                    temp = build_AST(token_table , *j , end-1);
                    body_node->tail->next = temp;
                    body_node->tail = temp;
                    *j = end+1;
                }
            }

        }
        

        *start = body_end;
        root->for_node = for_node;
        root->body = body_node;

        return root;
    }

    return NULL;
}
