#ifndef COMPILER_H
#define COMPILER_H

#include"compiler_result.h"
#include"preprocessor.h"
#include"semantic.h"
#include"TACcode.h"
#include"ASMCODE.h"
#include"VM.h"


extern CompilerResult result;

typedef struct{
    VM vm;
    Semantic_ctxt context;
    REGISTER registers;
}COMPILER;

static int prepare_source(const char *file_name , char lines[MAX_LINES][MAX_LINE_LEN] , const char* exp[MAX_LINES] , int *lines_count);

static int is_func_def(int index);

static int parse_function(int *i , int *start ,COMPILER *compiler);

static int is_if_statement(int index);

static int parse_if_statement(int *i , int *start ,COMPILER *compiler);

static int is_while_statement(int index);

static int parse_while(int *i , int *start ,COMPILER *compiler);

static int is_for_statement(int index);

static int parse_for(int *i , int *start , COMPILER *compiler);

static void parse_program(COMPILER *compiler);

static void parse_return(int *start , int *i , COMPILER *compiler);

static void parse_declaration_(int *start , int *assign_pos , char **name);

int compile_file(const char *file_name , COMPILER *compiler);

int compiler_source(const char* source);

#endif