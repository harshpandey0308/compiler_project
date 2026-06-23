#include<stdio.h>
#include<stdbool.h>
#include"TACcode.h"
#include"semantic.h"
#include"VM.h"

float  registers[4] = {0};
char  reg_name[4][5] = {"R0" , "R1" , "R2" , "R3"};

int SP = 0;

stack_entry vm_stack[1000];



mem_entry vm_memory[1000];
int mem_count = 0;

float RET_VAL = 0;

int PC = 0;


Label_entry label_table[100];
int label_count = 0;

void BUILD_LABEL_TABLE(){
    for(int i=0 ; i<tac_count ; i++){
        if(tac_table[i].type == TAC_LABEL || tac_table[i].type == TAC_FUNC_BEGIN){
            strcpy(label_table[label_count].label , tac_table[i].label);
            label_table[label_count].index = i;
            label_count++;
        }
    }
}

int find_label(char *target){
    for(int i=0 ; i<label_count ; i++){
        if(strcmp(label_table[i].label , target) == 0){
            return label_table[i].index;
        }
    }
    return -1;
}

float get_name(char *val){
    if(isdigit(val[0]) || val[0] == '-'){
        float fvalue = atof(val);
        return fvalue;
    }
    else if(strcmp(val , "RETVAL") == 0){
        return RET_VAL;
    }

    else{
        int i = 0;
        while(i<mem_count && strcmp(vm_memory[i].name , val) != 0){
            i++;
        }
        if(i<mem_count){
            return vm_memory[i].value;
        }
        printf("Variable '%s' not found in the memory.\n",val);
        return 0.0;
    }
}

void set_name(char *name , float value){
    for(int i=0 ; i<mem_count ; i++){
        if(strcmp(vm_memory[i].name , name) == 0){
            vm_memory[i].value = value;
            return;
        }
        
    }
    strcpy(vm_memory[mem_count].name , name);
    vm_memory[mem_count].value = value;
    mem_count++;
    return;

}

void run_vm(){
    PC = 0;
    while(PC < tac_count){
        TAC instr = tac_table[PC];
        switch (instr.type){
            case TAC_ASSIGN:{
                if(strcmp(instr.op2 , "") == 0){
                    float val = get_name(instr.op1);
                    set_name(instr.result , val);
                }
                else{
                    float op1_val = get_name(instr.op1);
                    float op2_val = get_name(instr.op2);
                    float result = 0;
                    if(strcmp(instr.opr , "+") == 0) result = op1_val + op2_val;
                    else if(strcmp(instr.opr , "-") == 0) result = op1_val - op2_val;
                    else if(strcmp(instr.opr , "*") == 0) result = op1_val*op2_val;
                    else if(strcmp(instr.opr , "/") == 0) result = op1_val/op2_val;
                    set_name(instr.result , result);
                }
                
                break;
            }
            
            case TAC_IF_GOTO:{
                float val1 = get_name(instr.op1);
                float val2 = get_name(instr.op2);

                int cond = 0;
                if(strcmp(instr.opr , "<") == 0) cond = (val1 < val2);
                else if(strcmp(instr.opr , ">") == 0) cond = (val1 > val2);
                else if(strcmp(instr.opr , "<=") == 0) cond = (val1 <= val2);
                else if(strcmp(instr.opr , ">=") == 0) cond = (val1 >= val2);
                else if(strcmp(instr.opr , "==") == 0) cond = (val1 == val2);
                else if(strcmp(instr.opr , "!=") == 0) cond = (val1 != val2);

                if(cond){
                    int index;
                    index = find_label(instr.label);
                    PC = index;
                    continue;
                }
                
                break;
            }

            case TAC_GOTO:{
                int ind = find_label(instr.label);
                PC = ind;
                continue;
            }

            case PARAM:{
                float op1_value = get_name(instr.op1);
                sprintf(vm_stack[SP].data , "%f" , op1_value);
                vm_stack[SP].is_label = 0;
                SP++;
                break;
            }
            
            case FUNC_CALL:{
                char *func_name = instr.op1;
                int param_count = 0;
                int i=0;
                while(i<sym_count){
                    if(sym_table[i].is_param == 1 && strcmp(sym_table[i].scope , func_name) == 0){
                        param_count++;
                    }                   
                    i++;
                }
                char ret_label[50];
                SP--;
                strcpy(ret_label , vm_stack[SP].data);
                

                float args[10] ;
                for(int i=param_count-1 ; i>=0 ;i--){
                    SP--;
                    args[i] = atof(vm_stack[SP].data);
                }

                int param_index = 0 ;
                
                for(int j=0 ; j<sym_count ; j++){
                    if(sym_table[j].is_param == 1 && strcmp(sym_table[j].scope , func_name) == 0){
                        set_name(sym_table[j].sym , args[param_index]);
                        param_index++;
                    }
                }
                
                strcpy(vm_stack[SP].data , ret_label);
                vm_stack[SP].is_label = 1;
                SP++;

                PC = find_label(func_name);
                continue;
            }
            case RETURN:{
                float value1 = get_name(instr.op1);
                RET_VAL = value1;

                SP--;
                char ret_lab[50];
                strcpy(ret_lab , vm_stack[SP].data);

                PC = find_label(ret_lab);
                continue;
            }

            case TAC_LABEL:
            case TAC_FUNC_BEGIN:
                break;

            case TAC_PUSH:{
                float value = get_name(instr.op1);
                sprintf(vm_stack[SP].data , "%f" , value);
                vm_stack[SP].is_label = 0;
                SP++;
                break;
            }

            case TAC_POP:{
                SP--;
                float pop_val = get_name(vm_stack[SP].data);
                set_name(instr.result , pop_val);
                break;
            }

            case TAC_JMP_DYNAMIC:{
                SP--;
                char value[50];
                strcpy(value , vm_stack[SP].data);
                PC = find_label(value);
                continue;
            }

            default:
                printf("Unknown Instruction type %d at PC = %d.\n",instr.type , PC);
                break;
        }
        PC++;
    }
}
