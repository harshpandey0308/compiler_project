#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include"TACcode.h"

float eval_TAC(char* op1 , char* op2 , char* opr);

void constant_fold();

void Const_propagate();

void dead_code();

void print_TAC();

void optimizer();

#endif // OPTIMIZER_H