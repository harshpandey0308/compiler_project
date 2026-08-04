#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include"TACcode.h"

float eval_TAC(const char* op1 ,const char* op2 ,const  char* opr);

void constant_fold(TACProgram *program);

void Const_propagate(TACProgram *program);

void dead_code(TACProgram *program);

void append_TAC(char *buffer , const char* text);

void BUILD_TAC_TEXT(char *buffer ,const TACProgram *program);

const char* TAC_text(char *buffer);

#endif // OPTIMIZER_H