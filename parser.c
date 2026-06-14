#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include"tokenizer.h"
#include"parser.h"

NODE* create_node(char *exp){
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
    if(start == end){
        return create_node(tokens[start].value);
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