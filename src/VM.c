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
            //printf("ADDED : label = %s and index = %d with label type : %d\n",tac_table[i].label , i , tac_table[i].type);
        }
    }
    //printf("the total label found : %d\n",lab_count);
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
        //printf("the value is %f\n",fvalue);
        return fvalue;
    }
    else if(strcmp(val , "RETVAL") == 0){
        //printf("the return value is %f\n", RET_VAL);
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
        //printf("The variable is %s\n",val);
        //printf("Variable '%s' not found in the memory.\n",val);
        return 0.0;
    }
}

void set_name(char *name , float value){
    for(int i=0 ; i<mem_count ; i++){
        if(strcmp(vm_memory[i].name , name) == 0){
            //printf("Setting value of %s to %f at index %d\n", name, value, i);
            vm_memory[i].value = value;

            return;
        }
        
    }
    strcpy(vm_memory[mem_count].name , name);
    vm_memory[mem_count].value = value;
    //printf("Adding new variable %s with value %f to memory at index %d\n", name, value, mem_count);
    mem_count++;
    return;

}

void set_by_index(int addr , float value){
    if(addr >= 0 && addr < mem_count){
        vm_memory[addr].value = value;
    }
    else{
        printf("Invalid memory address: %d\n", addr);
    }
}

void handle_printf(int arg_count){
    char ret_value[50];
    SP--;
    strcpy(ret_value , vm_stack[SP].data);

    //printf("DEBUG RET_LABEL : %s\n",ret_value);
    char arg_arr[10][50];
    for(int i=arg_count-1 ; i>=0 ; i--){
        SP--;
        strcpy(arg_arr[i] , vm_stack[i].data);
        
    }
    
    //printf("c\n");
    char *fmt = arg_arr[0];
    //printf("DEBUG fmt='%s'\n", fmt);
    //printf("DEBUG arg_arr[1]='%s'\n", arg_arr[1]);
    int arg_indx = 1;
    for(int i=0 ; fmt[i] != '\0' ; i++){
        //printf("%d\n",i);
        if(fmt[i] == '%'){
            i++;
            if(fmt[i] == 'd'){
                int arg = (int)atof(arg_arr[arg_indx++]);
                printf("%d",arg);
            }
            else if(fmt[i] == 'f'){
                float arg1 = (int)atof(arg_arr[arg_indx++]);
                printf("%f",arg1);
            }
            else if(fmt[i] == 'c'){
                char arg2 = (char)atof(arg_arr[arg_indx++]);
                printf("%c",arg2);
            }
            else if(fmt[i] == 's'){
                char arg3[10];
                strcpy(arg3 , arg_arr[arg_indx++]);
                printf("%s",arg3);
            }
        }
        else if(fmt[i] == '\\'){
            i++;
            if(fmt[i] == 'n'){
                printf("\n");
            }
            else if(fmt[i] == 't'){
                printf("\t");
            }
        }
        else{
            //printf("ab\n");
            printf("%c",fmt[i]);
        }
    }

    strcpy(vm_stack[SP].data , ret_value);
    vm_stack[SP].is_label = 1;
    SP++;

    PC = find_label(ret_value);

}

void handle_scanf(int arg_count){
    //printf("a\n");
    SP--;
    char ret_label[50];
    strcpy(ret_label , vm_stack[SP].data);
    
    //printf("DEBUG RETURN LABEL :%s\n",ret_label);
    char arg_arr[100][50];
    for(int i=arg_count-1 ; i>=0 ; i--){
        SP--;
        strcpy(arg_arr[i] , vm_stack[SP].data);
    }

    //printf("c\n");

    char *fmt = arg_arr[0];
    //printf("fmt = %s\n",fmt);
    int arr_ind = 1;
    for(int i=0 ; fmt[i] != '\0' ; i++){
        //printf("d\n");
        if(fmt[i] == '%'){
            i++;
            int addr = atoi(arg_arr[arr_ind++]);

            if(fmt[i] == 'd'){
                int i_value;
                scanf("%d",&i_value);
                vm_memory[addr].value = (float)i_value;
                printf("value : %f\n",vm_memory[addr].value);
            }
            else if(fmt[i] == 'f'){
                float f_value;
                scanf("%f",&f_value);
                vm_memory[addr].value = f_value;
            }
            else if(fmt[i] == 'c'){
                float c_val;
                scanf("%c",&c_val);
                vm_memory[addr].value = (float)c_val;
            }
            //printf("scanf completed.\n");
        }
        
    }

    //strcpy(vm_stack[SP].data , ret_label);
    //vm_stack[SP].is_label = 1;
    //SP++;

    //printf("label at the top of stack : %s\n",ret_label);

    PC = find_label(ret_label);
    //printf("After scanf, SP=%d\n", SP);
}

void run_vm(){
    PC = find_label("main");
    //printf("TAC_COUNT:%d\n",tac_count);
    while(PC < tac_count && SP >= 0){
        TAC instr = tac_table[PC];
        //printf("instr.type = %d\n",instr.type);
        switch (instr.type){
            case TAC_ASSIGN:{
                //printf("instr.result = %s , instr.op1 = %s , instr.opr = %s , instr.op2 = %s.\n",instr.result , instr.op1 ,instr.opr , instr.op2);
                //printf("TAC_ASSIGN: %s = %s %s %s\n",instr.result , instr.op1 , instr.opr , instr.op2);
                if(instr.is_deref_write == 1){
                    //printf("dereference write operation detected for %s = %s %s \n",instr.result , instr.op1 , instr.opr);
                    int addr1 = (int)get_name(instr.result);
                    //printf("addr1 = %d\n",addr1);
                    float val1 = get_name(instr.op1);
                    //printf("the right side value = %f\n",val1);
                    set_by_index(addr1 , val1);
                    break;
                }

                if(strcmp(instr.op2 , "") == 0){
                    //printf("TAC_ASSIGN: %s = %s\n",instr.result , instr.op1);
                    if(strcmp(instr.opr , "&") == 0){
                        int addr = -1;
                        //printf("address of operation detected for %s = %s\n",instr.result , instr.op1);
                        for(int i=0 ; i<mem_count ; i++){
                            if(strcmp(vm_memory[i].name , instr.op1) == 0){
                                //printf("found the address of %s at index %d\n",instr.op1 , i);
                                addr = i;
                                //printf("addr of %s is %d\n",instr.op1 , addr);
                                break;
                            }
                        }
                        set_name(instr.result , (float)addr);
                    }
                    else if(strcmp(instr.opr , "*") == 0){
                        float ptr_val;
                        //printf("dereference read operation detected for %s = %s\n",instr.result , instr.op1);
                        for(int j=0 ; j<mem_count ; j++){
                            //printf("checking memory at index %d with name %s\n",j , vm_memory[j].name);
                            //printf("checking if %s == %s\n",vm_memory[j].name , instr.op1);
                            if(strcmp(vm_memory[j].name , instr.op1) == 0){
                                int val = (int)vm_memory[j].value;
                                //printf("found the pointer value of %s at index %d with value %f\n",instr.op1 , j , vm_memory[j].value);
                                //printf("dereferencing pointer to get value at address %d\n",val);
                                ptr_val = vm_memory[val].value;
                                //printf("value at addr %d is %f\n",val , ptr_val);
                                break;
                            }
                        }
                        set_name(instr.result , ptr_val);
                    }
                    else{
                        float val = get_name(instr.op1);
                        //printf("TAC_ASSIGN is : %s = %f\n",instr.result , val);
                        set_name(instr.result , val);
                        //printf("TAC_ASSIGN: %f\n",val);
                    }
                }

                else if(sym_table[PC].size>0){
                    for(int i=0 ; i<sym_table[PC].size ; i++){
                        char arr_slot[50];
                        sprintf(arr_slot , "%s%d" , sym_table[PC].sym , i);
                        set_name(arr_slot , 0.0);
                    }
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
                    //printf("TAC_ASSIGN: %f\n", result);
                }
                
                break;
            }
            
            case TAC_IF_GOTO:{
                //printf("PC value at TAC IF GOTO:%d\n",PC);
                //printf("instr.op1 = %s , instr.op2 = %s\n",instr.op1 , instr.op2);
                float val1;
                float val2;
                float ptr_val1;
                float ptr_val2;
                int cond = 0;
                
                if(instr.op1[0] == '*' && isalpha(instr.op1[1]) && isdigit(instr.op2[0])){
                    char ptr_name[50];
                    strcpy(ptr_name , instr.op1 + 1);
                    int p_index = (int)get_name(ptr_name);
                    ptr_val1 = vm_memory[p_index].value;
                    ptr_val2 = get_name(instr.op2);
                    //printf("ptr_val1 = %s , ptr_val2 = %f.\n",ptr_val1 , ptr_val2);
                    if(strcmp(instr.opr , "<") == 0) cond = (ptr_val1 < ptr_val2);
                    else if(strcmp(instr.opr , ">") == 0) cond = (ptr_val1 > ptr_val2);
                    else if(strcmp(instr.opr , "<=") == 0) cond = (ptr_val1 <= ptr_val2);
                    else if(strcmp(instr.opr , ">=") == 0) cond = (ptr_val1 >= ptr_val2);
                    else if(strcmp(instr.opr , "==") == 0) cond = (ptr_val1 == ptr_val2);
                    else if(strcmp(instr.opr , "!=") == 0) cond = (ptr_val1 != ptr_val2);
                }
                else{
                    val1 = get_name(instr.op1);
                    val2 = get_name(instr.op2);
                    if(strcmp(instr.opr , "<") == 0) cond = (val1 < val2);
                    else if(strcmp(instr.opr , ">") == 0) cond = (val1 > val2);
                    else if(strcmp(instr.opr , "<=") == 0) cond = (val1 <= val2);
                    else if(strcmp(instr.opr , ">=") == 0) cond = (val1 >= val2);
                    else if(strcmp(instr.opr , "==") == 0) cond = (val1 == val2);
                    else if(strcmp(instr.opr , "!=") == 0) cond = (val1 != val2);
                }

                if(cond){
                    int index;
                    index = find_label(instr.label);
                    //printf("TAC_IF_GOTO: %d\n",index);
                    PC = index;
                    continue;
                }
                break;
            }

            case TAC_GOTO:{
                int ind = find_label(instr.label);
                //printf("TAC_GOTO: %d\n",ind);
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
                //printf("PARAM: %f\n",op1_value);
                break;
            }
            
            case FUNC_CALL:{
                char *func_name = instr.op1;
                //printf("func name = %s\n",func_name);
                int param_count = 0;

                int arg_count = atoi(instr.op2);

                if(strcmp(func_name , "printf") == 0){
                    handle_printf(arg_count);
                    continue;
                }

                if(strcmp(func_name , "scanf") == 0){
                    //printf("scanf is going to perform.\n");
                    handle_scanf(arg_count);
                    continue;
                }

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
                //printf("ret_label:%s\n",ret_label);
                vm_stack[SP].is_label = 1;
                SP++;
                //printf("AFTER PUSHING RETURN LABEL BACK:SP = %d\n",SP);

                PC = find_label(func_name);
                //printf("JUMPING TO PC = %d FOR FUNCTION:%s\n",PC , func_name);
                continue;
            }

            case TAC_PARAM_STRING:{
                strcpy(vm_stack[SP].data , instr.op1);
                vm_stack[SP].is_label = 0;
                SP++;
                break;
            }

            case TAC_PARAM_ADDR:{
                int addr = -1;
                for(int i=0 ; i<mem_count ; i++){
                    if(strcmp(vm_memory[i].name , instr.op1) == 0){
                        addr = i;
                        break;
                    }
                }

                if(addr == -1){
                    set_name(instr.op1 , 0.0);
                    addr = mem_count - 1;
                }

                char addr_str[50];
                sprintf(addr_str , "%d" , addr);
                strcpy(vm_stack[SP].data , addr_str);
                vm_stack[SP].is_label = 0;
                SP++;
                break;
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
        //printf("PC:%d\n",PC);
        if(PC<0){
            break;
        }
    }
}

char *find_type(char *name){
    //printf("the name is : %s\n",name);
    for(int i=0 ; i<sym_count ; i++){
        if(strcmp(sym_table[i].sym , name) == 0){
            return sym_table[i].type;
        }
    }
    return "UNKNOWN";
}
void print_vm_memory(){
    printf("\n-----VM_MEMORY_STATE-----\n");
    for(int i=0 ; i<mem_count ; i++){
        if(vm_memory[i].name[0] == 't' && isdigit(vm_memory[i].name[1])){
            continue;
        }
        if(strcmp(vm_memory[i].name , "RETVAL") == 0){
            continue;
        }

        char *type = find_type(vm_memory[i].name);

        if(strcmp(type , "int") == 0){
            printf("%15s = %d\n",vm_memory[i].name , (int)vm_memory[i].value);
        }
        else if(strcmp(type , "float") == 0){
            printf("%15s = %f\n",vm_memory[i].name , vm_memory[i].value);
        }
        else if(strcmp(type , "char") == 0){
            printf("%15s = %c\n",vm_memory[i].name , (char)vm_memory[i].value);
        }
        else if(strcmp(type , "UNKNOWN") == 0){
            printf("THE VALUE IS UNKNOWN\n");
        }
    }
    printf("RET_VAL = %f\n",RET_VAL);
}