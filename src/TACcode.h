#ifndef TACCODE_H
#define TACCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include"parser.h"
#include"tokenizer.h"

#define TAC_ASSIGN 0
#define TAC_IF_GOTO 1
#define TAC_GOTO 2
#define TAC_LABEL 3
#define PARAM 4
#define FUNC_CALL 5
#define RETURN 6
#define TAC_PUSH 7
#define TAC_POP 8
#define TAC_JMP_DYNAMIC 9
#define TAC_FUNC_BEGIN 10
#define TAC_PARAM_STRING 11
#define TAC_PARAM_ADDR 12

typedef struct tac{
    char result[50];
    char op1[50];
    char op2[50];
    char opr[5];
    char label[10];
    char scope[50];
    int is_dead;
    int type;
    int is_deref_write;
    int is_addr;
}TAC;

extern TAC tac_table[100];
extern int tac_count;

char* new_temp();

char* Label();

void emit_ASSIGN(char* result , char* op1 , char* op2 , char* opr);

void emit_IF_GOTO(char* op1 , char* opr , char* op2 , char* label);

void emit_GOTO(char* label);

void emit_LABEL(char* label);

void emit_PARAM(char* value);

void emit_CALL(char* name , int arg_count);

void emit_RETURN(char* value);

void emit_FUNC_BEG(char *name);

void emit_param_string(char *str);

void emit_param_addr(char *value);

void Generate_if_tac(TOKEN tokens[] , int if_pos);

void Generate_while_tac(TOKEN tokens[] , int while_pos);

void Generate_for_TAC(TOKEN tokens[] , int for_pos);

char* Generate_TAC(NODE* node);

//void print_TAC();

#endif // TACCODE_H