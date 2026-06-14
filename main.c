#include<stdio.h>
#include<stdlib.h>
#include"tokenizer.h"
#include"parser.h"
#include"semantic.h"
#include"TACcode.h"
#include"optimizer.h"
#include"ASMCODE.h"
#include"preproceesor.h"

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

    for(int i=0 ; i<lines_count ; i++){
        //printf("checking :'%s' -> is_declared : %d\n", lines[i] , is_declared(lines[i]) );
        if(is_declared(lines[i])){
            parse_declaration(lines[i]);
        }
    }

    print_sym();

    const char* exp[MAX_LINES];
    for(int i=0 ; i<lines_count ; i++){
        exp[i] = lines[i];
    }

    const int n = lines_count;

    lexer(exp , &n);

    int start = 0;
    for(int i=0 ; i<token_count ; i++){
        

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
                        printf("After if-else body %d : %s\n",i , tokens[i].value);
                        break;
                    }
                } 
                i++;
            }
            printf("i is at token %d : %s\n",i , tokens[i].value);
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
            printf("for statement found at token %d\n",i);
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
            //printf("DEBUG: start = %d , value = %s , tokentype = %d\n",start , tokens[start].value , tokens[start].tokentype);
            if(tokens[start].tokentype == KEYWORD){
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

            Check_Undeclared(root);
            Type_check(root);

            Generate_TAC(root);

            //print_TAC();

            free_tree(root);
            start = i+1;

        }
    }

    printf("\nBefore optimization :\n");
    print_TAC();

    constant_fold();
    Const_propagate();
    dead_code();

    printf("\nAfter optimization :\n");
    print_TAC();

    Generate_code();

    /*printf("program started\n");
    const char* exp[]={"float a;",
                "float b;",
                "float c;" ,
                "float d;" ,
                "int e;" ,
                "float f;" ,
                "a = (b + d*e - (c/7) + f%6);"};
    
    printf("running.\n");

    const int n = sizeof(exp)/sizeof(exp[0]);
    //lexer(exp);
    root = parser(exp , &n);

    printf("After parsing\n");
    printf("Syntax tree created.\n");

    semantic(exp , root , &n);
    printf("\nSymbol table:\n");

    Generate_TAC(root);

    optimizer();

    Generate_code();

    */
    //free_tree(root);
    printf("program ended\n");

    return 0;
}