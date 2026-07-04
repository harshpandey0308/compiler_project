#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"tokenizer.h"
#include"parser.h"

NODE* create_node(char *exp){
    NODE* new = (NODE*)malloc(sizeof(NODE));
    strcpy(new->value,exp);
    //printf("the new node contain %s .\n",new->value);
    new->left = NULL;
    new->right = NULL;
    new->ARG_count = 0;
    new->is_Call = 0;
    new->is_addr_of = 0;
    new->is_deref = 0;
    for(int i=0 ; i<50 ; i++){
        new->ARG[i] = NULL;
    }
    return new;
}

void print(NODE* root){
    if(root == NULL){
        return;
    }
    //printf("Inorder traversal of the syntax tree:\n");
    print(root->left);
    printf(" %s ",root->value);
    print(root->right);
    //printf("\n syntax tree printed.\n");
}

int find_operator(TOKEN tokens[] , int start , int end){
    int depth = 0;

    for(int i=end ; i>=start ; i--){
        if (strcmp(tokens[i].value , ")")==0)
        {
            depth++;
        }
        else if(strcmp(tokens[i].value , "(")==0){
            depth--;
        }
        else if(depth == 0 && tokens[i].tokentype == OPERATOR && strcmp(tokens[i].value , "=")==0){
            return i;
        }
    }

    depth = 0; 

    for(int i=end ; i>=start ; i--){
        if (strcmp(tokens[i].value , ")")==0)
        {
            depth++;
        }
        else if(strcmp(tokens[i].value ,"(")==0){
            depth--;
        }
        else if(depth == 0 && tokens[i].tokentype == OPERATOR && (strcmp(tokens[i].value , "+") == 0 || strcmp(tokens[i].value , "-") == 0)){
            return i;
        }
    }

    depth = 0; 

    for(int i=end ; i>=start ; i--){
        if (strcmp(tokens[i].value , ")")==0)
        {
            depth++;
        }
        else if(strcmp(tokens[i].value , "(")==0){
            depth--;
        }
        else if(depth == 0 && tokens[i].tokentype == OPERATOR && (strcmp(tokens[i].value , "*")==0 || strcmp(tokens[i].value , "/")==0 || strcmp(tokens[i].value , "%")==0)){
            return i;
        }
    }
    return -1;
}

NODE* Build_AST(TOKEN tokens[] , int start , int end){
    //printf("Building AST for tokens from %d to %d.\n",start , end);
    if(start == end){
        return create_node(tokens[start].value);
    }

    if(tokens[start].tokentype == IDENTIFIER && (strcmp(tokens[start+1].value , "[") == 0)){
        //printf("the tokens are %s at %d , %s at %d , %s at %d.\n",tokens[start].value , start , tokens[start+1].value , start+1 , tokens[start+2].value , start+2);
        char arr_name[50];
        
        sprintf(arr_name ,"%s%s " ,tokens[start].value , tokens[start+2].value);

        if(start+4 <= end && strcmp(tokens[start+4].value , "=") == 0){
            NODE* arr_node = create_node(arr_name);

            NODE* root = create_node(tokens[start+4].value);

            root->left = arr_node;
            root->right = Build_AST(tokens , start+5 , end);

            return root;
        }
        else{
            return create_node(arr_name);
        }
        //printf("array name : %s\n",arr_name);
    }

    if(strcmp(tokens[start].value , "&") == 0){
        //printf("address of operator detected for %s\n",tokens[start+1].value);
        NODE* addr_node = create_node(tokens[start+1].value);
        //NODE* left = create_node(tokens[start+1].value);
        //addr_node->left = left;
        addr_node->is_addr_of = 1;
        return addr_node;
    }

    if(strcmp(tokens[start].value , "*") == 0 && tokens[start+1].tokentype == IDENTIFIER ){
        if(start + 2 <= end && strcmp(tokens[start+2].value , "=") == 0){
            NODE* deref_node = create_node(tokens[start+2].value);
            NODE* left_node = create_node(tokens[start+1].value);
            //NODE* left_l_node = create_node(tokens[start+1].value);
            //left_node->left = left_l_node;
            left_node->is_deref = 1;
            deref_node->left = left_node;
            deref_node->right = Build_AST(tokens , start+3 , end);
            return deref_node;
        }
        else{
            NODE* deref_node = create_node(tokens[start+1].value);
            deref_node->is_deref = 1;
            return deref_node;
        }
        
    }

    if(tokens[start].tokentype == FUNC_NAME){
        NODE* call_node = create_node(tokens[start].value);
        call_node->is_Call = 1;
        call_node->ARG_count = 0;
        
        int arg_pos = start + 2;

        while(strcmp(tokens[arg_pos].value , ")") != 0){
            int arg_end = arg_pos;
            if(strcmp(tokens[arg_end].value , ",") != 0 && strcmp(tokens[arg_end].value , ")") != 0){
                arg_end++;
            }

            call_node->ARG[call_node->ARG_count++] = Build_AST(tokens , arg_pos , arg_end-1);

            if(strcmp(tokens[arg_end].value , ",") == 0){
                arg_pos = arg_end + 1;
            }
            else{
                break;
            }
        }
        return call_node;
    }

    int is_wrapped = 1;
    
    if(strcmp(tokens[start].value , "(") != 0 && strcmp(tokens[end].value , ")") != 0){
        is_wrapped = 0;
    }
    else{
        int depth = 0;
        for(int i=start ; i<=end ; i++){
            if(strcmp(tokens[i].value , "(")==0){
            depth++;
            }
            else if(strcmp(tokens[i].value , ")")==0){
            depth--;
            }
            if(depth == 0 && i<end){
            is_wrapped = 0;
            break;
            } 
        }
    }
    

    if(is_wrapped){
        return Build_AST(tokens , start+1 , end-1);
    }
    //if(start<=end && tokens[start].value == '(' && tokens[end].value == ')'){
      //  return Build_AST(tokens , start+1 , end-1);
    //}

    int pos = find_operator(tokens , start , end);

    if(pos == -1){
        return create_node(tokens[start].value);
    }

    NODE* root = create_node(tokens[pos].value);
    printf("the operator is %s at position %d\n",tokens[pos].value , pos);
    root->left = Build_AST(tokens , start , pos-1);
    root->right = Build_AST(tokens , pos+1 , end);

    return root;
}

/*NODE *parser(const char* exp[] , const int* n){
    
    //TOKEN tokens[50];
    printf("Parsing started.\n");

    token_count = lexer(exp , n);

    printf("After tokenization\n");

    int start = 0;
    NODE* root = NULL;

    for(int i=0 ; i<token_count ; i++){

        if(strcmp(tokens[i].value , ";")== 0){

            if(strcmp(tokens[i].value , "=")==0){
            start = i + 1;
            continue;
            }
            
            root = Build_AST(tokens , start , i-1);

            printf("Syntax tree for statement %d.\n", i );

            print(root);

            start = i + 1;

        }

        
    }

    

    
    printf("\nParsing completed.\n");
    
    return root;
}*/