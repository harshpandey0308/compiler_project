#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include"parser.h"
#include"semantic.h"
#include"TACcode.h"
#include"optimizer.h"
#include"ASMCODE.h"
#include"preprocessor.h"
#include"VM.h"
#include"compiler.h"

NODE* root;
CompilerResult result;

static int prepare_source(const char *file_name , char lines[MAX_LINES][MAX_LINE_LEN] , const char* exp[MAX_LINES] , int *lines_count , COMPILER *compiler){
    FILE *file;
    
    file = fopen(file_name, "r");

    if(file == NULL){
        //printf("Error opening file %s.\n", file_name);
        return 0;
    }

    *lines_count = preprocesses(file_name, lines);

    if(*lines_count == 0) return 0;

    printf("File read : %d lines.\n", *lines_count);

    //

    //print_sym();

    for(int i=0 ; i<*lines_count ; i++){
        exp[i] = lines[i];
        //printf("content of lines are %c.\n",exp[i]);
    }

    const int *n = lines_count;

    lexer(exp , n , compiler->token_table);

    fclose(file);

    printf("file closed.\n");

    return 1;

}

static int is_func_def(int index , COMPILER *compiler){
    if(index+2 >= compiler->token_table->token_count){
        return 0;
    }
    if(compiler->token_table->tokens[index].tokentype != TOKEN_KEYWORD)
        return 0;

    if(compiler->token_table->tokens[index+1].tokentype != TOKEN_FUNCTION && compiler->token_table->tokens[index+1].tokentype != TOKEN_IDENTIFIER){
        return 0;
    }
    if(strcmp(compiler->token_table->tokens[index+2].lexeme , "(") != 0){
        return 0;
    }

    return 1;
}

static int parse_function(int *i , int *start , COMPILER *compiler){
    if(!is_func_def(*i , compiler)){
        return 0;
    }

    char* func_name = compiler->token_table->tokens[*i+1].lexeme;
    DataType ret_type;
    if(strcmp(compiler->token_table->tokens[*i].lexeme , "int") == 0){
            ret_type = TYPE_INT;
        }
        else if(strcmp(compiler->token_table->tokens[*i].lexeme , "char") == 0){
            ret_type = TYPE_CHAR;
        }
        else if(strcmp(compiler->token_table->tokens[*i].lexeme , "float") == 0){
            ret_type = TYPE_FLOAT;
        }
        else if(strcmp(compiler->token_table->tokens[*i].lexeme , "double") == 0){
            ret_type = TYPE_DOUBLE;
        }
        else{
            ret_type = TYPE_VOID;
        }

    add_symbol(&compiler->context , func_name , ret_type , 0 , 0);

    strcpy(compiler->context.current_scope , func_name);

    emit_FUNC_BEG(&compiler->vm.program , func_name );

    while(*i<compiler->token_table->token_count && strcmp(compiler->token_table->tokens[*i].lexeme , "(") != 0){
        (*i)++;
    }
    int param_start = *i + 1;
    int param_end = param_start;

    while(*i<compiler->token_table->token_count && strcmp(compiler->token_table->tokens[param_end].lexeme , ")") != 0){
        DataType param_type;
        if(strcmp(compiler->token_table->tokens[*start].lexeme , "int") == 0){
            param_type = TYPE_INT;
        }
        else if(strcmp(compiler->token_table->tokens[*start].lexeme , "char") == 0){
            param_type = TYPE_CHAR;
        }
        else if(strcmp(compiler->token_table->tokens[*start].lexeme , "float") == 0){
            param_type = TYPE_FLOAT;
        }
        else if(strcmp(compiler->token_table->tokens[*start].lexeme , "double") == 0){
            param_type = TYPE_DOUBLE;
        }
        else{
            param_type = TYPE_VOID;
        }
        char *param_name = compiler->token_table->tokens[param_end+1].lexeme;
        //printf("Adding parameter %s of type %s  of %s function to symbol table\n", param_name, param_type , context->current_scope);
        add_symbol(&compiler->context, param_name , param_type , 1 , 0);
        param_end += 2;
        if(strcmp(compiler->token_table->tokens[param_end].lexeme , ",") == 0){
            param_end++;
        }
    }
    *i = param_end + 1;

    *start = *i+1;

    return 1;
}

static int is_if_statement(int index , COMPILER *compiler){
    //printf("checking the statement include if or not.\n");
    if(compiler->token_table->tokens[index].tokentype == TOKEN_KEYWORD && (strcmp(compiler->token_table->tokens[index].lexeme , "if") == 0)){
        //printf("yes the statement include if keyword.\n");
        return 1;
    }
    //printf("no , does not include if.\n");
    return 0;
}

static int parse_if_statement(int *i , int *start , COMPILER *compiler){
    //printf("checking if statement and lexeme of i = %d.\n",*i);
    if(is_if_statement(*i , compiler)){
            //printf("if statement found at token %d\n",*i);
            Generate_if_tac(compiler->token_table , *i , &compiler->vm.program);

            while(*i<compiler->token_table->token_count && !(compiler->token_table->tokens[*i].tokentype == TOKEN_KEYWORD && strcmp(compiler->token_table->tokens[*i].lexeme , "else") == 0)){
                (*i)++;
            }

            int depth = 0;
            while(*i<compiler->token_table->token_count){
                if(strcmp(compiler->token_table->tokens[*i].lexeme , "{") == 0) depth++;
                if(strcmp(compiler->token_table->tokens[*i].lexeme , "}") == 0){
                    depth--;
                    if(depth == 0){
                        (*i)++;
                        //printf("After if-else body %d : %s\n",*i , compiler->token_table->tokens[*i].lexeme);
                        break;
                    }
                } 
                (*i)++;
            }
            //printf("i is at token %d : %s\n",*i , compiler->token_table->tokens[*i].lexeme);
            (*i)--;
            *start = *i;
            return 1;
    }
    return 0;
}

static int is_while_statement(int index , COMPILER *compiler){
    if(compiler->token_table->tokens[index].tokentype == TOKEN_KEYWORD && strcmp(compiler->token_table->tokens[index].lexeme , "while") == 0){
        //printf("while included.\n");
        return 1;
    }
    //printf("while not included.\n");
    return 0;
}

static int parse_while(int *i , int *start , COMPILER *compiler){
    //printf("checking the while statement.\n");
    if(is_while_statement(*i , compiler)){
        //printf("while statement found at token %d\n",*i);
        Generate_while_tac(compiler->token_table , *i , &compiler->vm.program);

        //printf("while body ends at token %d\n",*i);

        while(*i<compiler->token_table->token_count && strcmp(compiler->token_table->tokens[*i].lexeme , "{") != 0){
            (*i)++;
        }

        int depth1 = 0;
        while(*i<compiler->token_table->token_count){
            if(strcmp(compiler->token_table->tokens[*i].lexeme , "{") == 0) depth1++;
            if(strcmp(compiler->token_table->tokens[*i].lexeme , "}") == 0){
                depth1--;
                if(depth1 == 0){
                    (*i)++;
                    break;
                }
            }
            (*i)++;
        }
        //printf("After while body %d : %s\n",*i , compiler->token_table->tokens[*i].lexeme);
        *start = *i;
        return 1;
    }

    return 0;
}

static int is_for_statement(int index , COMPILER *compiler){
    if(compiler->token_table->tokens[index].tokentype == TOKEN_KEYWORD && strcmp(compiler->token_table->tokens[index].lexeme , "for") == 0){
        //printf("included.\n");
        return 1;
    }
    //printf("not included.\n");
    return 0;
}

static int parse_for(int *i , int *start , COMPILER *compiler){
    if(is_for_statement(*i , compiler)){
        //printf("for statement found at token %d\n",*i);
        Generate_for_TAC(compiler->token_table , *i , &compiler->vm.program);

        while(*i<compiler->token_table->token_count && strcmp(compiler->token_table->tokens[*i].lexeme , "{") != 0){
            (*i)++;
        }

        int depth3 = 0;
        while(*i<compiler->token_table->token_count){
            if(strcmp(compiler->token_table->tokens[*i].lexeme , "{") == 0) depth3++;
            if(strcmp(compiler->token_table->tokens[*i].lexeme , "}") == 0){
                depth3--;
                if(depth3 == 0){
                    (*i)++;
                    break;
                }
            }
            (*i)++;
        }

        *start = *i;
        return 1;
    }
    
    return 0;
}

static void parse_return(int *start , int *i ,COMPILER *compiler){
    char* ret_val = compiler->token_table->tokens[*start+1].lexeme;
    //printf("RETURN VALUE: %s\n",compiler->token_table->tokens[start+1].lexeme);
    emit_RETURN(&compiler->vm.program , ret_val);
    *start = *i+1;
}

static void parse_declaration_(int *start , int *assign_pos , char **name , COMPILER *compiler){
    *name = compiler->token_table->tokens[*start+1].lexeme;
    if(strcmp(compiler->token_table->tokens[*start+2].lexeme , "=") == 0){
        *assign_pos = *start + 2;
    }
}

static int parse_statement(int *i , int *start , COMPILER *compiler){
    //printf("parsing statement check...\n");
    if(strcmp(compiler->token_table->tokens[*i].lexeme , ";")==0){
            if(compiler->token_table->tokens[*start].tokentype == TOKEN_KEYWORD && strcmp(compiler->token_table->tokens[*start].lexeme , "return") == 0){
                parse_return(start , i , compiler);
                return 1;
            }
            //printf("DEBUG: start = %d , lexeme = %s , tokentype = %d\n",*start , compiler->token_table->tokens[*start].lexeme , compiler->token_table->tokens[*start].tokentype);
            if(compiler->token_table->tokens[*start].tokentype == TOKEN_KEYWORD){
                //printf("checking whether the token is keyword or not.\n");
                DataType type;
                if(strcmp(compiler->token_table->tokens[*start].lexeme , "int") == 0){
                    type = TYPE_INT;
                }
                else if(strcmp(compiler->token_table->tokens[*start].lexeme , "char") == 0){
                    type = TYPE_CHAR;
                }
                else if(strcmp(compiler->token_table->tokens[*start].lexeme , "float") == 0){
                    type = TYPE_FLOAT;
                }
                else if(strcmp(compiler->token_table->tokens[*start].lexeme , "double") == 0){
                    type = TYPE_DOUBLE;
                }
                else{
                    type = TYPE_VOID;
                }
                char* name = NULL;
                int assign_pos = -1;
                int *pos_assign = &assign_pos;

                int size = 0;

                if(strcmp(compiler->token_table->tokens[*start+1].lexeme , "*") == 0){
                    name =  compiler->token_table->tokens[*start + 2].lexeme;
                    //printf("pointer declaration detected for %s of type %d\n",name , type);
                    
                    if(strcmp(compiler->token_table->tokens[*start + 3].lexeme , "=") == 0){
                        //printf("declaration with initialization detected for %s of type %s\n",name , type);
                        *pos_assign = *start + 3;
                        //printf("assign position  = %d\n", assign_pos);
                    }
                }

                else if(strcmp(compiler->token_table->tokens[*start + 2].lexeme , "[") == 0){
                    name = compiler->token_table->tokens[*start+1].lexeme;
                    size = atoi(compiler->token_table->tokens[*start+3].lexeme);
                }
                else{
                    parse_declaration_(start , pos_assign , &name , compiler);
                }

                //printf("assign position  = %d\n", assign_pos);

                //add_symbol(name , type  , context->current_scope , 0 , 0);
                //printf("the compiler->token_table->tokens of type %s is %s.\n",compiler->token_table->tokens[start].lexeme , compiler->token_table->tokens[start+1].lexeme);
                //printf("add symbol %s of type %s of %s function.\n",name , type , context->current_scope);
                add_symbol(&compiler->context , name , type , 0 , size);
                //printf("symbols added.\n");

                
                if(assign_pos != -1){
                    //printf("declaration with initialization detected for %s of type %s\n",name , type);
                    //printf("assign_pos = %d , end = %d\n",assign_pos , i-1);
                    NODE* decl_AST = build_AST(compiler->token_table , assign_pos-1 , (*i)-1);
                    Check_Undeclared(decl_AST , &compiler->context);
                    Type_check(decl_AST , &compiler->context);
                    Generate_TAC(decl_AST , &compiler->vm.program);

                    free_tree(decl_AST);
                }
                *start = *i+1;
                return 1;
            }

            if(*start >= *i){
                *start = *i+1;
                return 1;
            }

            //printf("Generating TAC for statement from token %d to %d.\n", start, i);
            root = build_AST(compiler->token_table , *start , (*i)-1);
            //printf("Syntax tree for statement %d.\n", i);
            //print(root);

            Check_Undeclared(root , &compiler->context);
            Type_check(root , &compiler->context);
            //printf("\ntac generation.\n");
            Generate_TAC(root , &compiler->vm.program);

            //BUILD_TAC_TEXT(result.TAC_buffer);

            free_tree(root);
            *start = *i+1;
            return 1;
        }
        else{
            //printf("NO parse statement.\n");
            return 0;
        }
}

static void parse_program(COMPILER *compiler){
    //printf("parse program starts.\n");
    int start = 0;
    for(int i=0 ; i<compiler->token_table->token_count ; i++){
        
        if(strcmp(compiler->context.current_scope , "global") != 0 && strcmp(compiler->token_table->tokens[i].lexeme , "}") == 0){
            strcpy(compiler->context.current_scope , "global");
            //printf("%s\n",compiler->context.current_scope);
            start = i + 1;
            continue;
        }

        if(parse_function(&i , &start , compiler)){
            //printf("parsing function .\n");
            continue;
        }
        //printf("parsing if .\n");

        if(parse_if_statement(&i , &start , compiler)){
            //printf("parsing if statement and i = %d.\n",i);
            continue;
        }

        //printf("while detection\n");

        if(parse_while(&i , &start , compiler)){
            //printf("parsing while statement.\n");
            continue;
        }
        //printf("for detection : token=%s , type =  %d\n", compiler->token_table->tokens[i].lexeme, compiler->token_table->tokens[i].tokentype);

        if(parse_for(&i , &start , compiler)){
            //printf("parsing for statement.\n");
            continue;
        }

        //printf("parsing statements  .. \n");
        if(parse_statement(&i , &start , compiler)){
            //printf("parsing statement.\n");
            continue;
        }
    }
}



int compile_file(const char *file_name , COMPILER *compiler){
    //printf("entering the compiler.\n");
    strcpy(compiler->context.current_scope , "global");

    for(int i=0 ; i<REG_COUNT ; i++){
        compiler->registers.reg_free[i] = 1;
    }

    char lines[MAX_LINES][MAX_LINE_LEN];
    const char* exp[MAX_LINES];
    int lines_count;

    if(!prepare_source(file_name , lines , exp , &lines_count , compiler)){
        return 1;
    }

    //printf("parsing started.\n");
    parse_program(compiler);

    print_sym(&compiler->context.symbols);
    

    //printf("\nBefore optimization :\n");
    //print_TAC();

    constant_fold(&compiler->vm.program);
    Const_propagate(&compiler->vm.program);
    dead_code(&compiler->vm.program);

    //printf("\nAfter optimization :\n");
    //BUILD_TAC_TEXT(result.TAC_buffer);

    Generate_code(&compiler->vm.program , &compiler->registers);

    //printf("\n----VM EXECUTION----\n");

    compiler->vm.symbol = compiler->context.symbols;

    BUILD_LABEL_TABLE(&compiler->vm);
    run_vm(&compiler->vm);
    printf("printing virtual machine computation.\n");

    print_vm_memory(&compiler->vm);

    //printf("program ended\n");

    return 0;
}