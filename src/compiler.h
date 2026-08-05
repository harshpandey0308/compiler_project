#include"compiler_result.h"
#include"preprocessor.h"

#ifndef COMPILER_H
#define COMPILER_H

extern CompilerResult result;

static int prepare_source(const char *file_name , char lines[MAX_LINES][MAX_LINE_LEN] , const char* exp[MAX_LINES] , int *lines_count);

static int is_func_def(int index);

static int parse_function(int *i , int *start , Semantic_ctxt *context ,  TACProgram *program);

static int is_if_statement(int index);

static int parse_if_statement(int *i , int *start , TACProgram *program);

static int is_while_statement(int index);

static int parse_while(int *i , int *start , TACProgram *program);

static int is_for_statement(int index);

static int parse_for(int *i , int *start , TACProgram *program);

static void parse_program(Semantic_ctxt *context , TACProgram *program);

static void parse_return(int *start , int *i , TACProgram *program);

static void parse_declaration_(int *start , int *assign_pos , char **name);

int compile_file(const char *file_name ,  Semantic_ctxt *context , TACProgram *program , const SymbolTable *table , REGISTER *registers , VM *vm);

int compiler_source(const char* source);

#endif