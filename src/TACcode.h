#ifndef TACCODE_H
#define TACCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include"parser.h"
#include"tokenizer.h"

#define MAX_TAC_INSTR_SIZE 1000
#define MAX_OPERAND_SIZE 50
#define MAX_OPERATOR_LENGTH 10
#define MAX_LENGTH 50
typedef enum{
    TAC_ASSIGN, 
    TAC_IF_GOTO, 
    TAC_GOTO, 
    TAC_LABEL, 
    PARAM, 
    FUNC_CALL, 
    RETURN, 
    TAC_PUSH, 
    TAC_POP, 
    TAC_JMP_DYNAMIC, 
    TAC_FUNC_BEGIN, 
    TAC_PARAM_STRING, 
    TAC_PARAM_ADDR, 
}TAC_type;
typedef struct tac{
    char result[50];
    char op1[MAX_OPERAND_SIZE];
    char op2[MAX_OPERAND_SIZE];
    char opr[MAX_OPERATOR_LENGTH];
    char label[MAX_LENGTH];
    char scope[MAX_LENGTH];
    int is_dead;
    TAC_type type;
    int is_deref_write;
    int is_addr;
    int is_char_lit;
}TAC;

typedef struct{
    TAC code[MAX_TAC_INSTR_SIZE];
    int tac_count;
}TACProgram;

char* new_temp_name();

char* new_label();

void emit_ASSIGN(TACProgram *program , char* result , char* op1 , char* op2 , char* opr);

void emit_IF_GOTO(TACProgram *program , char* op1 , char* opr , char* op2 , char* label);

void emit_GOTO(TACProgram *program, char* label);

void emit_LABEL(TACProgram *program , char* label);

void emit_PARAM(TACProgram *program , char* value);

void emit_CALL(TACProgram *program , char* name , int arg_count);

void emit_RETURN(TACProgram *program , char* value);

void emit_FUNC_BEG(TACProgram *program , char *name);

void emit_param_string(TACProgram *program , char *str);

void emit_param_addr(TACProgram *program , char *value);

int Generate_if_tac(TokenEntry *token_table, int if_pos , TACProgram *program);

int Generate_while_tac(TokenEntry *token_table , int while_pos , TACProgram *program);

int Generate_for_TAC(TokenEntry *token_table , int for_pos , TACProgram *program);

char* Generate_TAC(NODE* node , TACProgram *program);

void print_TAC(TACProgram *program);

//void print_TAC();

#endif // TACCODE_H