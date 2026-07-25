#include"compiler_result.h"

#ifndef COMPILER_H
#define COMPILER_H

extern CompilerResult result;

static int prepare_source(const char *file_name , char lines[MAX_LINES][MAX_LINE_LEN] , const char* exp[MAX_LINES] , int *lines_count);

static int is_func_def(int index);

static int parse_function(int *i , int *start);

static void parse_program();

int compile_file(const char *file_name);

int compiler_source(const char* source);

#endif