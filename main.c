#include<stdio.h>
#include<stdlib.h>
#include"tokenizer.h"
#include"parser.h"
#include"semantic.h"
#include"TACcode.h"
#include"optimizer.h"
#include"ASMCODE.h"
#include"preproceesor.h"
#include"VM.h"

NODE* root;

int main(int argc, char* argv[]){

    FILE* file;

    if(argc < 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    file = fopen(argv[1], "r");

    if(file == NULL){
        printf("Error opening file %s.\n", argv[1]);
        return 1;
    }

    char lines[MAX_LINES][MAX_LINE_LEN];
    int lines_count = preprocesses(argv[1], lines);

    if(lines_count == 0) return 1;

    printf("File read : %d lines.\n", lines_count);

    //

    //print_sym();

    const char* exp[MAX_LINES];
    for(int i=0 ; i<lines_count ; i++){
        exp[i] = lines[i];
        //printf("content of lines are %c.\n",exp[i]);
    }

    const int n = lines_count;

    lexer(exp , &n);

    int start = 0;
    for(int i=0 ; i<token_count ; i++){
        
        if(strcmp(Current_Scope , "global") != 0 && strcmp(tokens[i].value , "}") == 0){
            strcpy(Current_Scope , "global");
            printf("%s\n",Current_Scope);
            start = i + 1;
            continue;
        }

        if(tokens[i].tokentype == KEYWORD && (tokens[i+1].tokentype == FUNC_NAME || tokens[i+1].tokentype == IDENTIFIER) && strcmp(tokens[i+2].value , "(") == 0){
            char* func_name = tokens[i+1].value;
            char* ret_type = tokens[i].value;

            add_symbol(func_name , ret_type  , "global" , 0 , 0);

            strcpy(Current_Scope , func_name);

            emit_FUNC_BEG(func_name);

            while(i<token_count && strcmp(tokens[i].value , "(") != 0){
                i++;
            }
            int param_start = i + 1;
            int param_end = param_start;

            while(i<token_count && strcmp(tokens[param_end].value , ")") != 0){
                char *param_type = tokens[param_end].value;
                char *param_name = tokens[param_end+1].value;
                //printf("Adding parameter %s of type %s  of %s function to symbol table\n", param_name, param_type , Current_Scope);
                add_symbol(param_name , param_type , Current_Scope , 1 , 0);
                param_end += 2;
                if(strcmp(tokens[param_end].value , ",") == 0){
                    param_end++;
                }
            }
            i = param_end + 1;


            int body_start = i;
            
            start = i+1;
            continue;

        }

        if(tokens[i].tokentype == KEYWORD && (strcmp(tokens[i].value , "if") == 0)){
            //printf("if statement found at token %d\n",i);
            Generate_if_tac(tokens , i);

            while(i<token_count && !(tokens[i].tokentype == KEYWORD && strcmp(tokens[i].value , "else") == 0)){
                i++;
            }

            int depth = 0;
            while(i<token_count){
                if(strcmp(tokens[i].value , "{") == 0) depth++;
                if(strcmp(tokens[i].value , "}") == 0){
                    depth--;
                    if(depth == 0){
                        i++;
                        //printf("After if-else body %d : %s\n",i , tokens[i].value);
                        break;
                    }
                } 
                i++;
            }
            //printf("i is at token %d : %s\n",i , tokens[i].value);
            i--;
            start = i;
            continue;
        }

        //
        //printf("while detection\n");

        if(tokens[i].tokentype == KEYWORD && strcmp(tokens[i].value , "while") == 0){
                //printf("while statement found at token %d\n",i);
            Generate_while_tac(tokens , i);

                //printf("while body ends at token %d\n",i);

            while(i<token_count && strcmp(tokens[i].value , "{") != 0){
                i++;
            }

            int depth1 = 0;
            while(i<token_count){
                if(strcmp(tokens[i].value , "{") == 0) depth1++;
                if(strcmp(tokens[i].value , "}") == 0){
                    depth1--;
                    if(depth1 == 0){
                        i++;
                        break;
                    }
                }
                i++;
            }
                //printf("After while body %d : %s\n",i , tokens[i].value);
            start = i;
            continue;
        }

        //printf("for detection : token=%s , type =  %d , KEYWORD=%d\n", tokens[i].value, tokens[i].tokentype, KEYWORD);

        if(tokens[i].tokentype == KEYWORD && strcmp(tokens[i].value , "for") == 0){
            //printf("for statement found at token %d\n",i);
            Generate_for_TAC(tokens , i);

            while(i<token_count && strcmp(tokens[i].value , "{") != 0){
                i++;
            }

            int depth3 = 0;
            while(i<token_count){
                if(strcmp(tokens[i].value , "{") == 0) depth3++;
                if(strcmp(tokens[i].value , "}") == 0){
                    depth3--;
                    if(depth3 == 0){
                        i++;
                        break;
                    }
                }
                i++;
            }

            start = i;
            continue;
        }

        if(strcmp(tokens[i].value , ";")==0){
            if(tokens[start].tokentype == KEYWORD && strcmp(tokens[start].value , "return") == 0){
                char* ret_val = tokens[start+1].value;
                printf("RETURN VALUE: %s\n",tokens[start+1].value);
                emit_RETURN(ret_val);
                start = i+1;
                continue;
            }
            //printf("DEBUG: start = %d , value = %s , tokentype = %d\n",start , tokens[start].value , tokens[start].tokentype);
            if(tokens[start].tokentype == KEYWORD){
                char* type = tokens[start].value;
                char* name = tokens[start+1].value;

                printf("the tokens of type %s is %s.\n",tokens[start].value , tokens[start+1].value);
                int size = 0;

                if(strcmp(tokens[start + 2].value , "[") == 0){
                    size = atoi(tokens[start+3].value);
                }
                //printf("add symbol %s of type %s of %s function.\n",name , type , Current_Scope);
                add_symbol(name , type , Current_Scope , 0 , size);
                //printf("symbols added.\n");

                if(strcmp(tokens[start + 2].value , "=") == 0){
                    NODE* decl_AST = Build_AST(tokens , start+1 , i-1);
                    Check_Undeclared(decl_AST , Current_Scope);
                    Type_check(decl_AST , Current_Scope);
                    Generate_TAC(decl_AST);

                    free_tree(decl_AST);
                }
                start = i+1;
                continue;
            }

            if(start >= i){
                start = i+1;
                continue;
            }

            root = Build_AST(tokens , start , i-1);
            printf("Syntax tree for statement %d.\n", i);
            print(root);

            Check_Undeclared(root , Current_Scope);
            Type_check(root , Current_Scope);
            printf("\ntac generation.\n");
            Generate_TAC(root);

                //print_TAC();

            free_tree(root);
            start = i+1;

        }
    }

    print_sym();
    

    printf("\nBefore optimization :\n");
    print_TAC();

    constant_fold();
    Const_propagate();
    dead_code();

    printf("\nAfter optimization :\n");
    print_TAC();

    Generate_code();

    printf("\n----VM EXECUTION----\n");

    BUILD_LABEL_TABLE();
    run_vm();

    print_vm_memory();

    printf("program ended\n");

    return 0;
}