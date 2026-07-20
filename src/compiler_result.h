#ifndef COMPLER_RESULT_H
#define COMPILER_RESULT_H

#pragma once

typedef struct compiler_result{
    char output_buffer[50000];
    char AST_buffer[50000];
    char sym_buffer[50000];
    char TAC_buffer[50000];
    char VM_buffer[50000];
    char ASS_buffer[50000];
}CompilerResult;

#endif