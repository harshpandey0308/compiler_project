#ifndef VM_H
#define  VM_H

typedef struct{
    char data[50];
    int is_label;
}stack_entry;

typedef struct{
    char name[50];
    float value;
}mem_entry;

typedef struct{
    char label[50];
    int index;
}Label_entry;

void BUILD_LABEL_TABLE();

int find_label(char *target);

float get_name(char *val);

void set_name(char *name , float value);

void run_vm();

void print_vm_memory();

#endif
