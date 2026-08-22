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

static int prepare_source(const char *file_name , char lines[MAX_LINES][MAX_LINE_LEN] , const char* exp[MAX_LINES] , int *lines_count , COMPILER *compiler){
    FILE *file;
    
    file = fopen(file_name, "r");

    if(file == NULL){
        //printf("Error opening file %s.\n", file_name);
        return 0;
    }

    char buffer[1024];

    compiler->result.source_buffer[0] = '\0';

    while(fgets(buffer , sizeof(buffer) , file)){
        strcat(compiler->result.source_buffer , buffer);
    }

    *lines_count = preprocesses(file_name, lines);

    if(*lines_count == 0) return 0;

    printf("File read : %d lines.\n", *lines_count);

    //parse_declaration(lines , &compiler->context);

    //print_sym();

    for(int i=0 ; i<*lines_count ; i++){
        exp[i] = lines[i];
        //printf("content of lines are %c.\n",exp[i]);
    }

    const int *n = lines_count;

    printf("[GUI TEST] BEFORE LEXER.\n");
    fflush(stdout);

    lexer(exp , n , compiler->token_table);

    printf("[GUI TEST] AFTER LEXER.\n");
    fflush(stdout);

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

NODE *parse_function(int *i , int *start , COMPILER *compiler){
    if(!is_func_def(*i , compiler)){
        return NULL;
    }

    BODY *body = malloc(sizeof(BODY));

    body->head = NULL;
    body->tail = NULL;

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

    while(*i<compiler->token_table->token_count && strcmp(compiler->token_table->tokens[*i].lexeme , "(") != 0){
        (*i)++;
    }
    int param_start = *i + 1;
    int param_end = param_start;


    NODE *root = NULL;

    while(param_end<compiler->token_table->token_count && strcmp(compiler->token_table->tokens[param_end].lexeme , ")") != 0){
        DataType param_type;
        if(strcmp(compiler->token_table->tokens[param_end].lexeme , "int") == 0){
            param_type = TYPE_INT;
        }
        else if(strcmp(compiler->token_table->tokens[param_end].lexeme , "char") == 0){
            param_type = TYPE_CHAR;
        }
        else if(strcmp(compiler->token_table->tokens[param_end].lexeme , "float") == 0){
            param_type = TYPE_FLOAT;
        }
        else if(strcmp(compiler->token_table->tokens[param_end].lexeme , "double") == 0){
            param_type = TYPE_DOUBLE;
        }
        else{
            param_type = TYPE_VOID;
        }

        char *param_name = compiler->token_table->tokens[++param_end].lexeme;
        add_symbol(&compiler->context, param_name , param_type , 1 , 0);
        param_end += 1;

        if(strcmp(compiler->token_table->tokens[param_end].lexeme , ",") == 0){
            param_end++;
        }
    }

    root = create_node(func_name , AST_FUNCTION);

    *i = param_end + 1;

    int body_start = param_end+2;
    int body_end = body_start;

    int depth = 1;
    while(body_end < compiler->token_table->token_count && depth != 0){
        if(strcmp(compiler->token_table->tokens[body_end].lexeme , "{") == 0){
            depth++;
        }
        else if(strcmp(compiler->token_table->tokens[body_end].lexeme , "}") == 0){
            depth--;
        }
        body_end++;
    }

    NODE *temp = NULL;
    int k = body_start;
    while(k < compiler->token_table->token_count && k < body_end-1){
        if(strcmp(compiler->token_table->tokens[k].lexeme , "while") == 0){
            if(body->head == NULL){
                body->head = parse_loop(compiler->token_table , &k);
                body->tail = body->head;
            }
            else{
                temp = parse_loop(compiler->token_table , &k);
                body->tail->next = temp;
                body->tail = temp;
            }
        }
        else if(strcmp(compiler->token_table->tokens[k].lexeme , "for") == 0){
            if(body->head == NULL){
                body->head = parse_loop(compiler->token_table , &k);
                body->tail = body->head;
            }
            else{
                temp = parse_loop(compiler->token_table , &k);
                body->tail->next = temp;
                body->tail = temp;
            }
        }
        else if(strcmp(compiler->token_table->tokens[k].lexeme , "if") == 0){
            if(body->head == NULL){
                body->head = parse_cond(compiler->token_table , &k);
                body->tail = body->head;
            }
            else{
                temp = parse_cond(compiler->token_table , &k);
                body->tail->next = temp;
                body->tail = temp;
            }
        }
        else{
            int end = k;
            while(end < compiler->token_table->token_count && strcmp(compiler->token_table->tokens[end].lexeme , ";") != 0){
                end++;
            }
            *i = end;

            temp = parse_statement(i , &k , compiler);
            if(body->head == NULL){
                body->head = temp;
                body->tail = body->head;
            }
            else{
                body->tail->next = temp;
                body->tail = temp;
            }
            k = end + 1;
        }
    }

    root->body = body;

    *i = body_end;

    *start = body_end;

    return root;
}




NODE *parse_return(int *start , int *i ,COMPILER *compiler){
    NODE *root = create_node("return" , AST_RETURN);

    root->right = build_AST(compiler->token_table , *start + 1 , *i-1);
    *start = *i+1;

    return root;
}

static void parse_declaration_(int *start , int *assign_pos , char **name , COMPILER *compiler){
    //printf("start = %d\n",*start);
    //printf("name = %s\n",compiler->token_table->tokens[*start+1].lexeme);
    *name = compiler->token_table->tokens[*start+1].lexeme;
    if(strcmp(compiler->token_table->tokens[*start+2].lexeme , "=") == 0){
        *assign_pos = *start + 2;
    }
}

NODE *parse_statement(int *statement_i , int *start , COMPILER *compiler){
    if(strcmp(compiler->token_table->tokens[*statement_i].lexeme , ";")==0){
            if(compiler->token_table->tokens[*start].tokentype == TOKEN_KEYWORD && strcmp(compiler->token_table->tokens[*start].lexeme , "return") == 0){
                NODE *ret_node = parse_return(start , statement_i , compiler);
                return ret_node;
            }

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

                printf("assign position  = %d\n", assign_pos);

                printf("adding %s of %d in symbol table.\n",name , type);
                add_symbol(&compiler->context , name , type , 0 , size);

                printf("symbol count after add : %d\n",compiler->context.symbols.sym_count);

                NODE *declare_AST = NULL;
                if(assign_pos != -1){
                    declare_AST = build_AST(compiler->token_table , assign_pos-1 , (*statement_i)-1);
                }
                *start = *(statement_i)+1;
                return declare_AST;
            }

            if(*start >= *statement_i){
                *start = *(statement_i)+1;
                return NULL;
            }

            NODE *root = build_AST(compiler->token_table , *start , (*statement_i)-1);
            *start = *(statement_i)+1;

            return root;
        }

        else{
            return NULL;
        }
}

NODE *parse_program(COMPILER *compiler){
    //printf("parse program starts.\n");
    BODY *body = malloc(sizeof(BODY));

    body->head = NULL;
    body->tail = NULL;

    NODE *node = NULL;

    int start = 0;
    //printf("token count = %d.\n", compiler->token_table->token_count);
    for(int i=0 ; i<compiler->token_table->token_count ; i++){
        
        if(strcmp(compiler->context.current_scope , "global") != 0 && strcmp(compiler->token_table->tokens[i].lexeme , "}") == 0){
            strcpy(compiler->context.current_scope , "global");
            //printf("%s\n",compiler->context.current_scope);
            start = i + 1;
            continue;
        }

        if(strcmp(compiler->token_table->tokens[i].lexeme , "if") == 0){
            node = parse_cond(compiler->token_table , &i);
        }
        else if(strcmp(compiler->token_table->tokens[i].lexeme , "while") == 0){
            node = parse_loop(compiler->token_table , &i);
        }
        else if(strcmp(compiler->token_table->tokens[i].lexeme , "for") == 0){
            node = parse_loop(compiler->token_table , &i);
        }
        else if(is_func_def(i , compiler)){
            node = parse_function(&i , &start , compiler);

        }
        else{
            node = parse_statement(&i , &start , compiler);
        }

        if(node != NULL){
            if(body->head == NULL){
                body->head = node;
                body->tail = node;
            }
            else{
                body->tail->next = node;
                node->next = NULL;
                body->tail = node;
            }
            printf("HEAD : %s.\n",body->head->lexeme);
            printf("tail : %s.\n",body->tail->lexeme);
        }
        printf("i = %d.\n", i);
    }


    return body->head;
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
    
    //printf("symbol count = %d.\n",compiler->context.symbols.sym_count);
    //printf("parsing started.\n");
    NODE *program = parse_program(compiler);


    BUILD_SYMBOL_TEXT(&compiler->context.symbols , compiler->result.SYM_BUFFER);
    //printf("symbol count = %d.\n",compiler->context.symbols.sym_count);

    Check_Undeclared(program , &compiler->context);
    Type_check(program , &compiler->context);

    //printf("program  = %s.\n", program->lexeme);
    //printf("type = %d\n", program->type);
    
    Generate_TAC(program , &compiler->vm.program);
    //printf("\nBefore optimization :\n");
    //print_TAC(&compiler->vm.program);

    //constant_fold(&compiler->vm.program);
    //Const_propagate(&compiler->vm.program);
    //dead_code(&compiler->vm.program);

    //printf("\nAfter optimization :\n");

    BUILD_TAC_TEXT(compiler->result.TAC_buffer , &compiler->vm.program );

    //printf("printing tac");
    //print_TAC(&compiler->vm.program);
    printf("symbol count = %d.\n",compiler->context.symbols.sym_count);

    Generate_code(&compiler->vm.program , &compiler->registers , compiler->result.ASM_buffer);

    //printf("\n----VM EXECUTION----\n");

    compiler->vm.symbol = compiler->context.symbols;

    BUILD_LABEL_TABLE(&compiler->vm);
    run_vm(&compiler->vm , &compiler->result);
    printf("printing virtual machine computation.\n");

    BUILD_VM_TEXT(&compiler->vm , compiler->result.VM_buffer);

    printf("output buffer : \n");

    
    printf("%s.\n",compiler->result.output_buffer);

    //printf("program ended\n");

    return 0;
}