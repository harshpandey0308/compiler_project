#ifndef ASMCODE_H
#define ASMCODE_H

#include"TACcode.h"

#define REG_COUNT 4
#define MAX_REGISTER_MAP 200
#define MAX_STACK_ROW 1000
#define MAX_STACK_COLUMN 50
typedef struct REG_MAP{
    char temp[50];
    int register_index;
}REG_MAP;

typedef struct{
    REG_MAP register_map[MAX_REGISTER_MAP];
    int register_count;
    int reg_free[REG_COUNT];
}REGISTER;

typedef struct ASM_STACK{
    char STACK[MAX_STACK_ROW][MAX_STACK_COLUMN];
    int top;
}ASM_STACK;

int alloc_reg(REGISTER *registers);

void stack_push(ASM_STACK *stack , char *value);

char *stack_pop(ASM_STACK *stack);

void free_reg(int index , REGISTER *registers);

int find_reg(const char* temp , const REGISTER *registers);

void Generate_code(TACProgram *program , REGISTER *registers , char *buffer);

#endif // ASMCODE_H