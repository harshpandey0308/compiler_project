#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include"TACcode.h"

float eval_TAC(char* op1 , char* op2 , char* opr);

void constant_fold();

void Const_propagate();

void dead_code();

void append_TAC(const char* text);

void BUILD_TAC_TEXT(char *buffer);

void optimizer();

const char* TAC_text(char *buffer);

#endif // OPTIMIZER_H