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
int lab_count = 0;

void BUILD_LABEL_TABLE(){
    //printf("label table is building..\n");
    for(int i=0 ; i<tac_count ; i++){
        //printf("CHECKING LABEL TYPE..\n");
        if(tac_table[i].type == TAC_LABEL || tac_table[i].type == TAC_FUNC_BEGIN){
            //printf("found the label.\n");
            strcpy(label_table[lab_count].label , tac_table[i].label);
            label_table[lab_count].index = i;
            lab_count++;
            //printf("printing the table.\n");
            printf("ADDED : label = %s and index = %d with label type : %d\n",tac_table[i].label , i , tac_table[i].type);
        }
    }
    printf("the total label found : %d\n",lab_count);
}

int find_label(char *target){
    for(int i=0 ; i<lab_count ; i++){
        if(strcmp(label_table[i].label , target) == 0){
            return label_table[i].index;
        }
    }
    return -1;
}

float get_name(char *val){
    if(isdigit(val[0]) || val[0] == '-'){
        float fvalue = atof(val);
        printf("the value is %f\n",fvalue);
        return fvalue;
    }
    else if(strcmp(val , "RETVAL") == 0){
        printf("the return value is %f\n", RET_VAL);
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
        printf("The variable is %s\n",val);
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
    PC = find_label("main");
    printf("TAC_COUNT:%d\n",tac_count);
    while(PC < tac_count && SP >= 0){
        TAC instr = tac_table[PC];
        switch (instr.type){
            case TAC_ASSIGN:{
                if(strcmp(instr.op2 , "") == 0){
                    float val = get_name(instr.op1);
                    set_name(instr.result , val);
                    printf("TAC_ASSIGN: %f\n",val);
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
                    printf("TAC_ASSIGN: %f\n", result);
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
                    printf("TAC_IF_GOTO: %d\n",index);
                    PC = index;
                    continue;
                }
            }

            case TAC_GOTO:{
                int ind = find_label(instr.label);
                printf("TAC_GOTO: %d\n",ind);
                PC = ind;
                continue;
            }

            case PARAM:{
                float op1_value = get_name(instr.op1);
                //printf("SP AT PARAM : %d\n", SP);
                sprintf(vm_stack[SP].data , "%f" , op1_value);
                vm_stack[SP].is_label = 0;
                SP++;
                //printf("AFTER PARAM SP = %d\n",SP);
                printf("PARAM: %f\n",op1_value);
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
                //printf("BEFORE FUNCTION CALL:SP = %d\n",SP);
                char ret_label[50];
                SP--;
                if(SP<0){
                    break;
                }
                strcpy(ret_label , vm_stack[SP].data);
                //printf("AFTER POPPING RET_LAB : SP = %d and RET_LAB = %s\n",SP , ret_label);
                
                //printf("AFTER FUNC CALL : SP = %d\n",SP);
                float args[10] ;
                for(int i=param_count-1 ; i>=0 ;i--){
                    SP--;
                    args[i] = atof(vm_stack[SP].data);
                    if(SP<0){
                        printf("VALUE OF SP IS %d\n",SP);
                        break;
                    }
                }
                //printf("AFTER POP ARGS : SP = %d\n",SP);

                int param_index = 0 ;
                
                for(int j=0 ; j<sym_count ; j++){
                    if(sym_table[j].is_param == 1 && strcmp(sym_table[j].scope , func_name) == 0){
                        set_name(sym_table[j].sym , args[param_index]);
                        param_index++;
                    }
                }

                
                strcpy(vm_stack[SP].data , ret_label);
                printf("ret_label:%s\n",ret_label);
                vm_stack[SP].is_label = 1;
                SP++;
                //printf("AFTER PUSHING RETURN LABEL BACK:SP = %d\n",SP);

                PC = find_label(func_name);
                //printf("JUMPING TO PC = %d FOR FUNCTION:%s\n",PC , func_name);
                continue;
            }
            case RETURN:{
                float value1 = get_name(instr.op1);
                //printf("RETURN VALUE = %f and SP = %d\n",value1 , SP);
                //printf("STACK top = %s\n",vm_stack[SP-1].data);
                RET_VAL = value1;

                SP--;
                char ret_lab[50];
                strcpy(ret_lab , vm_stack[SP].data);
                //printf("RETURN , jumping to label %s\n",ret_lab);

                PC = find_label(ret_lab);
                //printf("RETURN PC = %d\n",PC);
                continue;
            }

            case TAC_LABEL:{
                break;
            }
            case TAC_FUNC_BEGIN:
                break;

            case TAC_PUSH:{
                strcpy(vm_stack[SP].data , instr.op1);

                if(instr.op1[0] == 'L' && isdigit(instr.op1[1])){
                    vm_stack[SP].is_label = 1;
                }
                else{
                    vm_stack[SP].is_label = 0;
                }
                
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
        printf("PC:%d\n",PC);
        if(PC<0){
            break;
        }
    }
}

void print_vm_memory(){
    printf("\n-----VM_MEMORY_STATE-----\n");
    for(int i=0 ; i<mem_count ; i++){
        printf("%-15s = %f\n",vm_memory[i].name , vm_memory[i].value);
    }
    printf("RET_VAL = %f\n",RET_VAL);
}