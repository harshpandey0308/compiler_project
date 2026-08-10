#ifndef COMPLER_RESULT_H
#define COMPILER_RESULT_H

#pragma once

typedef struct compiler_result{
    char output_buffer[50000];
    char SYM_BUFFER[50000];
    char TAC_buffer[50000];
    char VM_buffer[50000];
    char ASM_buffer[50000];

    char source_buffer[50000];
}CompilerResult;

#endif