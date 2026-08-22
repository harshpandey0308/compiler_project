#ifndef VM_H
#define  VM_H

#include"compiler_result.h"

#define MAX_VALUE_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_LABEL_LENGTH 50
#define MAX_STACK_SIZE 1000
#define MAX_MEMORY_SIZE 1000
#define MAX_LABEL_SIZE 100
typedef struct{
    char data[MAX_VALUE_LENGTH];
    int is_label;
}STACK_ENTRY;


typedef enum{
    INTEGER,
    FLOAT,
    CHARACTER,
    VOID
}member_tag;

typedef struct{
    union{
    int integer;
    float float_number;
    char character;
    }VALUE;

    DataType active_type;
}MEMBER;

typedef struct{
    STACK_ENTRY data[MAX_STACK_SIZE];
    int top;
}VMSTACK;

typedef struct{
    char name[MAX_NAME_LENGTH];
    DataType type;
    union{
        int int_val;
        float float_val;
        char ch_val;
    }value;
}MEMORY_ENTRY;

typedef struct{
    MEMORY_ENTRY data[MAX_MEMORY_SIZE];
    int memory_count;
}VM_MEMORY;

typedef struct{
    char label[MAX_LABEL_LENGTH];
    int index;
}LABEL_ENTRY;

typedef struct{
    LABEL_ENTRY data[MAX_LABEL_SIZE];
    int label_count;
}LABELS;


typedef struct{
    TACProgram program;
    VMSTACK vm_stack;
    VM_MEMORY memory;
    LABELS label_table;
    SymbolTable symbol;
    MEMBER member;

    float RET_VAL;
    float  registers[4];
    int PC;
}VM;

void BUILD_LABEL_TABLE(VM *vm);

int find_label(char *target , VM *VM);

MEMBER get_name(char *val , VM *vm);

void set_name(char *name , MEMBER *member , VM *vm);

void append_output(char *buffer , char *line);

void handling_printf(int arg_count , VM *vm , char *bufer);

void handle_scanf(int arg_count , VM *vm);

float member_to_float(MEMBER *member);

void run_vm(VM *vm , CompilerResult *result);

DataType find_type(char *name , VM *vm);

void append_vm(char *buffer , char *line);

void BUILD_VM_TEXT(VM *vm , char *buffer);

const char *output_text(char *buffer);

const char *VM_TEXT(char *buffer);

#endif
