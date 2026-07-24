#include"compiler_result.h"

#ifndef COMPILER_H
#define COMPILER_H

extern CompilerResult result;

int compile_file(const char *file_name);

int compiler_source(const char* source);

#endif