#ifndef VM_H
#define  VM_H

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

typedef struct{
    STACK_ENTRY data[MAX_STACK_SIZE];
    int top;
}VMSTACK;

typedef struct{
    char name[MAX_NAME_LENGTH];
    float value;
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
}LABEL;


typedef struct{
    TACProgram program;
    VMSTACK vm_stack;
    VM_MEMORY memory;
    LABEL label_table;
    SymbolTable symbol;

    float RET_VAL;
    float  registers[4];
    int PC;
}VM;

void BUILD_LABEL_TABLE(VM *vm);

int find_label(char *target , VM *VM);

float get_name(char *val , VM *vm);

void set_name(char *name , float value , VM *vm);

void handling_printf(int arg_count , VM *vm);

void handle_scanf(int arg_count , VM *vm);

void run_vm(VM *vm);

char *find_type(char *name , VM *vm);

void print_vm_memory(VM *vm);

#endif
