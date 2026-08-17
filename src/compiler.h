#ifndef COMPILER_H
#define COMPILER_H

#include"compiler_result.h"
#include"preprocessor.h"
#include"parser.h"
#include"semantic.h"
#include"TACcode.h"
#include"ASMCODE.h"
#include"VM.h"

typedef struct{
    TokenEntry *token_table;
    VM vm;
    Semantic_ctxt context;
    REGISTER registers;
    CompilerResult result;
}COMPILER;

static int prepare_source(const char *file_name , char lines[MAX_LINES][MAX_LINE_LEN] , const char* exp[MAX_LINES] , int *lines_count , COMPILER *compiler);

static int is_func_def(int index , COMPILER *compiler);

NODE* parse_function(int *i , int *start ,COMPILER *compiler);

NODE *parse_program(COMPILER *compiler);

NODE *parse_return(int *start , int *i , COMPILER *compiler);

static void parse_declaration_(int *start , int *assign_pos , char **name , COMPILER *compiler);

int compile_file(const char *file_name , COMPILER *compiler);

int compiler_source(const char* source);

#endif