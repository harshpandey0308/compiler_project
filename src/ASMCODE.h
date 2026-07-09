#ifndef ASMCODE_H
#define ASMCODE_H

#define REG_COUNT 4

typedef struct reg_map{
    char temp[50];
    int reg_index;
}REG_MAP;


extern REG_MAP regmap[100];
extern int reg_map_count;

int alloc_reg();

void stack_push(char *value);

char *stack_pop();

void free_reg(int index);

int find_reg(char* temp);

void Generate_code();

#endif // ASMCODE_H