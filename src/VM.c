#include<stdio.h>
#include<stdbool.h>
#include"TACcode.h"
#include"semantic.h"
#include"VM.h"

const char  reg_name[4][5] = {"R0" , "R1" , "R2" , "R3"};


void BUILD_LABEL_TABLE(VM *vm){
    vm->label_table.label_count = 0;
    //printf("label table is building..\n");
    for(int i=0 ; i<vm->program.tac_count ; i++){
        //printf("CHECKING LABEL TYPE..\n");
        if(vm->program.code[i].type == TAC_LABEL || vm->program.code[i].type == TAC_FUNC_BEGIN){
            //printf("found the label.\n");
            int pos = vm->label_table.label_count;
            strcpy(vm->label_table.data[pos].label , vm->program.code[i].label);
            vm->label_table.data[pos].index = i;
            vm->label_table.label_count++;
            //printf("printing the table.\n");
            //printf("ADDED : label = %s and index = %d with label type : %d\n",vm->program.code[i].label , i , vm->program.code[i].type);
        }
    }
    //printf("the total label found : %d\n",vm->label_table.label_count);
}

int find_label(char *target , VM *vm){
    //printf("the target is : %s\n",target);
    for(int i=0 ; i<vm->label_table.label_count ; i++){
        //printf("label in the table index %d is %s.\n",i,vm->label_table.data[i].label);
        if(strcmp(vm->label_table.data[i].label , target) == 0){
            return vm->label_table.data[i].index;
        }
    }
    return -1;
}

float get_name(char *val , VM *vm){
    if(val == NULL){
        return 0.0f;
    }

    if(isdigit((unsigned char)val[0]) || val[0] == '-'){
        float fvalue = atof(val);
        //printf("the value is %f\n",fvalue);
        return fvalue;
    }
    
    else if(strcmp(val , "RETVAL") == 0){
        //printf("the return value is %f\n", vm->RET_VAL);
        return vm->RET_VAL;
    }

    /*else if(strlen(val) == 1 ){
        int ascii = val[0];
        printf("ascii : %d\n",ascii);
        return (float)ascii;
    }*/

    else{
        int i = 0;
        while(i<vm->memory.memory_count && strcmp(vm->memory.data[i].name , val) != 0){
            i++;
        }
        if(i<vm->memory.memory_count){
            return vm->memory.data[i].value;
        }
        //printf("The variable is %s\n",val);
        //printf("Variable '%s' not found in the memory.\n",val);
        return 0.0f;
    }
}

void set_name(char *name , float value , VM *vm){
    for(int i=0 ; i<vm->memory.memory_count ; i++){
        if(strcmp(vm->memory.data[i].name , name) == 0){
            //printf("Setting value of %s to %f at index %d\n", name, value, i);
            vm->memory.data[i].value = value;

            return;
        }
        
    }
    strcpy(vm->memory.data[vm->memory.memory_count].name , name);
    vm->memory.data[vm->memory.memory_count].value = value;
    //printf("Adding new variable %s with value %f to memory at index %d\n", name, value, vm->memory.memory_count);
    vm->memory.memory_count++;
    return;

}

void set_by_index(int addr , float value , VM *vm){
    if(addr >= 0 && addr < vm->memory.memory_count){
        vm->memory.data[addr].value = value;
    }
    else{
        printf("Invalid memory address: %d\n", addr);
    }
}

void handle_printf(int arg_count , VM *vm){
    char ret_value[50];
    vm->vm_stack.top--;
    strcpy(ret_value ,vm->vm_stack.data[vm->vm_stack.top].data);

    //printf("DEBUG RET_LABEL : %s\n",ret_value);
    char arg_arr[10][50];
    for(int i=arg_count-1 ; i>=0 ; i--){
        vm->vm_stack.top--;
        strcpy(arg_arr[i] , vm->vm_stack.data[vm->vm_stack.top].data);
        //printf("stack data = %s and vm_stack->top = %d\n",vm_stack[vm_stack->top].data , vm_stack->top);
        
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
                float arg1 = atof(arg_arr[arg_indx++]);
                printf("%f",arg1);
            }
            else if(fmt[i] == 'c'){
                //printf("value is %f.\n",arg_arr[arg_indx]);
                //printf("index = %d\n", arg_indx);
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

    //strcpy(vm_stack[vm_stack->top].data , ret_value);
    //vm_stack[vm_stack->top].is_label = 1;
    //vm_stack->top++;
    //printf("returning to %s\n",ret_value);
    vm->PC = find_label(ret_value , vm);
    //printf("new PC = %d.\n", vm->PC);
}

void handle_scanf(int arg_count ,VM *vm){
    //printf("a\n");
    vm->vm_stack.top--;
    char ret_label[50];
    strcpy(ret_label ,vm->vm_stack.data[vm->vm_stack.top].data);
    
    //printf("DEBUG RETURN LABEL :%s\n",ret_label);
    char arg_arr[100][50];
    for(int i=arg_count-1 ; i>=0 ; i--){
        vm->vm_stack.top--;
        strcpy(arg_arr[i] , vm->vm_stack.data[vm->vm_stack.top].data);
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
                vm->memory.data[addr].value = (float)i_value;
                //printf("value : %f\n",memory->data[addr].value);
            }
            else if(fmt[i] == 'f'){
                float f_value;
                scanf("%f",&f_value);
                vm->memory.data[addr].value = f_value;
            }
            else if(fmt[i] == 'c'){
                float c_val;
                scanf("%c",&c_val);
                vm->memory.data[addr].value = (float)c_val;
            }
            //printf("scanf completed.\n");
        }
        
    }

    //strcpy(vm_stack[vm_stack->top].data , ret_label);
    //vm_stack[vm_stack->top].is_label = 1;
    //vm_stack->top++;

    //printf("label at the top of stack : %s\n",ret_label);

    vm->PC = find_label(ret_label , vm);
    //printf("After scanf, vm_stack->top=%d\n", vm_stack->top);
}

void run_vm(VM *vm){
    //printf("sym count =  %d.\n",vm->symbol.sym_count);
    vm->PC = find_label("main" , vm);
    //printf("TAC_COUNT:%d\n",vm->program.tac_count);
    while(vm->PC < vm->program.tac_count && vm->vm_stack.top >= 0){
        TAC instr = vm->program.code[vm->PC];
        //printf("instr.type = %d\n",instr.type);
        switch (instr.type){
            case TAC_ASSIGN:{
                //printf("PC in TAC ASSIGN = %d.\n",vm->PC);
                //printf("instr.result = %s , instr.op1 = %s , instr.opr = %s , instr.op2 = %s.\n",instr.result , instr.op1 ,instr.opr , instr.op2);
                //printf("TAC_ASSIGN: %s = %s %s %s\n",instr.result , instr.op1 , instr.opr , instr.op2);
                if(instr.is_deref_write == 1){
                    //printf("dereference write operation detected for %s = %s %s \n",instr.result , instr.op1 , instr.opr);
                    int addr1 = (int)get_name(instr.result , vm);
                    //printf("addr1 = %d\n",addr1);
                    float val1 = get_name(instr.op1 , vm);
                    //printf("the right side value = %f\n",val1);
                    set_by_index(addr1 , val1 , vm);
                    break;
                }

                if(strcmp(instr.op2 , "") == 0){
                    //printf("TAC_ASSIGN: %s = %s\n",instr.result , instr.op1);
                    if(strcmp(instr.opr , "&") == 0){
                        int addr = -1;
                        //printf("address of operation detected for %s = %s\n",instr.result , instr.op1);
                        for(int i=0 ; i<vm->memory.memory_count ; i++){
                            if(strcmp(vm->memory.data[i].name , instr.op1) == 0){
                                //printf("found the address of %s at index %d\n",instr.op1 , i);
                                addr = i;
                                //printf("addr of %s is %d\n",instr.op1 , addr);
                                break;
                            }
                        }
                        set_name(instr.result , (float)addr , vm);
                    }
                    else if(strcmp(instr.opr , "*") == 0){
                        float ptr_val;
                        //printf("dereference read operation detected for %s = %s\n",instr.result , instr.op1);
                        for(int j=0 ; j<vm->memory.memory_count ; j++){
                            //printf("checking memory at index %d with name %s\n",j , memory->data[j].name);
                            //printf("checking if %s == %s\n",memory->data[j].name , instr.op1);
                            if(strcmp(vm->memory.data[j].name , instr.op1) == 0){
                                int val = (int)vm->memory.data[j].value;
                                //printf("found the pointer value of %s at index %d with value %f\n",instr.op1 , j , memory->data[j].value);
                                //printf("dereferencing pointer to get value at address %d\n",val);
                                ptr_val = vm->memory.data[val].value;
                                //printf("value at addr %d is %f\n",val , ptr_val);
                                break;
                            }
                        }
                        set_name(instr.result , ptr_val , vm);
                    }
                    else{
                        float val;
                        //int n = strlen(instr.op1);
                    
                        val = get_name(instr.op1 , vm);
                        
                        //printf("string length : %d\n",n);
                        
                        //printf("instr.op1 = %s.\n",instr.op1);
                        //printf("TAC_ASSIGN is : %s = %f\n",instr.result , val);
                        set_name(instr.result , val , vm);
                        //printf("TAC_ASSIGN: %f\n",val);
                    }
                }

                else if(vm->symbol.table[vm->PC].size > 0){
                    for(int i=0 ; i<vm->symbol.table[vm->PC].size ; i++){
                        char arr_slot[50];
                        sprintf(arr_slot , "%s%d" , vm->symbol.table[i].sym , i);
                        set_name(arr_slot , 0.0 , vm);
                    }
                }

                else{
                    float op1_val = get_name(instr.op1 , vm);
                    float op2_val = get_name(instr.op2 , vm);
                    float result = 0;
                    if(strcmp(instr.opr , "+") == 0) result = op1_val + op2_val;
                    else if(strcmp(instr.opr , "-") == 0) result = op1_val - op2_val;
                    else if(strcmp(instr.opr , "*") == 0) result = op1_val*op2_val;
                    else if(strcmp(instr.opr , "/") == 0) result = op1_val/op2_val;
                    set_name(instr.result , result , vm);
                    //printf("TAC_ASSIGN: %f\n", result);
                }
                
                break;
            }
            
            case TAC_IF_GOTO:{
                //printf("vm->PC value at TAC IF GOTO:%d\n",vm->PC);
                //printf("instr.op1 = %s , instr.op2 = %s\n",instr.op1 , instr.op2);
                float val1;
                float val2;
                float ptr_val1;
                float ptr_val2;
                int cond = 0;
                
                if(instr.op1[0] == '*' && isalpha(instr.op1[1]) && isdigit(instr.op2[0])){
                    char ptr_name[50];
                    strcpy(ptr_name , instr.op1 + 1);
                    int p_index = (int)get_name(ptr_name , vm);
                    ptr_val1 = vm->memory.data[p_index].value;
                    ptr_val2 = get_name(instr.op2 , vm);
                    //printf("ptr_val1 = %s , ptr_val2 = %f.\n",ptr_val1 , ptr_val2);
                    if(strcmp(instr.opr , "<") == 0) cond = (ptr_val1 < ptr_val2);
                    else if(strcmp(instr.opr , ">") == 0) cond = (ptr_val1 > ptr_val2);
                    else if(strcmp(instr.opr , "<=") == 0) cond = (ptr_val1 <= ptr_val2);
                    else if(strcmp(instr.opr , ">=") == 0) cond = (ptr_val1 >= ptr_val2);
                    else if(strcmp(instr.opr , "==") == 0) cond = (ptr_val1 == ptr_val2);
                    else if(strcmp(instr.opr , "!=") == 0) cond = (ptr_val1 != ptr_val2);
                }
                else{
                    val1 = get_name(instr.op1 , vm);
                    val2 = get_name(instr.op2 , vm);
                    if(strcmp(instr.opr , "<") == 0) cond = (val1 < val2);
                    else if(strcmp(instr.opr , ">") == 0) cond = (val1 > val2);
                    else if(strcmp(instr.opr , "<=") == 0) cond = (val1 <= val2);
                    else if(strcmp(instr.opr , ">=") == 0) cond = (val1 >= val2);
                    else if(strcmp(instr.opr , "==") == 0) cond = (val1 == val2);
                    else if(strcmp(instr.opr , "!=") == 0) cond = (val1 != val2);
                }

                if(cond){
                    int index;
                    index = find_label(instr.label , vm);
                    //printf("TAC_IF_GOTO: %d\n",index);
                    vm->PC = index;
                    continue;
                }
                break;
            }

            case TAC_GOTO:{
                //printf("PC IN TAC_GOTO = %d.\n",vm->PC);
                int ind = find_label(instr.label , vm);
                //printf("TAC_GOTO: %d\n",ind);
                vm->PC = ind;
                continue;
            }

            case PARAM:{
                //printf("PC IN PARAM = %d.\n",vm->PC);
                float op1_value = get_name(instr.op1 , vm);
                //printf("vm_stack->top AT PARAM : %d\n", vm_stack->top);
                sprintf(vm->vm_stack.data[vm->vm_stack.top].data , "%f" , op1_value);
                vm->vm_stack.data[vm->vm_stack.top].is_label = 0;
                vm->vm_stack.top++;
                //printf("AFTER PARAM vm_stack->top = %d\n",vm_stack->top);
                //printf("PARAM: %f\n",op1_value);
                break;
            }
            
            case FUNC_CALL:{
                //printf("PC AT FUNCTION CALL = %d.\n",vm->PC);
                char *func_name = instr.op1;
                //printf("func name = %s\n",func_name);
                int param_count = 0;

                int arg_count = atoi(instr.op2);

                if(strcmp(func_name , "printf") == 0){
                    handle_printf(arg_count , vm);
                    continue;
                }

                if(strcmp(func_name , "scanf") == 0){
                    //printf("scanf is going to perform.\n");
                    handle_scanf(arg_count , vm);
                    continue;
                }

                int i=0;
                while(i<vm->symbol.sym_count){
                    if(vm->symbol.table[i].is_param == 1 && strcmp(vm->symbol.table[i].scope , func_name) == 0){
                        param_count++;
                    }                   
                    i++;
                }
                //printf("BEFORE FUNCTION CALL:vm_stack->top = %d\n",vm_stack->top);
                char ret_label[50];
                vm->vm_stack.top--;
                if(vm->vm_stack.top<0){
                    break;
                }
                strcpy(ret_label , vm->vm_stack.data[vm->vm_stack.top].data);
                //printf("AFTER POPPING RET_LAB : vm_stack->top = %d and RET_LAB = %s\n",vm_stack->top , ret_label);
                
                //printf("AFTER FUNC CALL : vm_stack->top = %d\n",vm_stack->top);
                float args[10] ;
                for(int i=param_count-1 ; i>=0 ;i--){
                    vm->vm_stack.top--;
                    args[i] = atof(vm->vm_stack.data[vm->vm_stack.top].data);
                    if(vm->vm_stack.top<0){
                        //printf("VALUE OF vm_stack->top IS %d\n",vm->vm_stack.top);
                        break;
                    }
                }
                //printf("AFTER POP ARGS : vm_stack->top = %d\n",vm_stack->top);

                int param_index = 0 ;
                printf("sym count = %d.\n",vm->symbol.sym_count);
                for(int j=0 ; j<vm->symbol.sym_count ; j++){
                    //printf("i = %d.\n",vm->symbol.sym_count);
                    if(vm->symbol.table[j].is_param == 1 && strcmp(vm->symbol.table[j].scope , func_name) == 0){
                        set_name(vm->symbol.table[j].sym , args[param_index] , vm);
                        param_index++;
                    }
                }

                
                strcpy(vm->vm_stack.data[vm->vm_stack.top].data , ret_label);
                //printf("ret_label:%s\n",ret_label);
                vm->vm_stack.data[vm->vm_stack.top].is_label = 1;
                vm->vm_stack.top++;
                //printf("AFTER PUSHING RETURN LABEL BACK:vm_stack->top = %d\n",vm_stack->top);

                vm->PC = find_label(func_name , vm);
                //printf("JUMPING TO vm->PC = %d FOR FUNCTION:%s\n",vm->PC , func_name);
                continue;
            }

            case TAC_PARAM_STRING:{
                //printf("PC AT PARAM STRING = %d.\n",vm->PC);
                strcpy(vm->vm_stack.data[vm->vm_stack.top].data , instr.op1);
                vm->vm_stack.data[vm->vm_stack.top].is_label = 0;
                vm->vm_stack.top++;
                break;
            }

            case TAC_PARAM_ADDR:{
                //printf("PC AT PARAM ADDR = %d.\n",vm->PC);
                int addr = -1;
                for(int i=0 ; i<vm->memory.memory_count ; i++){
                    if(strcmp(vm->memory.data[i].name , instr.op1) == 0){
                        addr = i;
                        break;
                    }
                }

                if(addr == -1){
                    set_name(instr.op1 , 0.0 , vm);
                    addr = vm->memory.memory_count - 1;
                }

                char addr_str[50];
                sprintf(addr_str , "%d" , addr);
                strcpy(vm->vm_stack.data[vm->vm_stack.top].data , addr_str);
                vm->vm_stack.data[vm->vm_stack.top].is_label = 0;
                vm->vm_stack.top++;
                break;
            }

            case RETURN:{
                //printf("PC AT RETURN = %d.\n",vm->PC);
                float value1 = get_name(instr.op1 , vm);
                //printf("RETURN VALUE = %f and vm_stack->top = %d\n",value1 , vm_stack->top);
                //printf("STACK top = %s\n",vm_stack[vm_stack->top-1].data);
                vm->RET_VAL = value1;

                vm->vm_stack.top--;
                char ret_lab[50];
                strcpy(ret_lab , vm->vm_stack.data[vm->vm_stack.top].data);
                //printf("RETURN , jumping to label %s\n",ret_lab);

                vm->PC = find_label(ret_lab , vm);
                //printf("RETURN vm->PC = %d\n",vm->PC);
                continue;
            }

            case TAC_LABEL:{
                break;
            }
            case TAC_FUNC_BEGIN:
                break;

            case TAC_PUSH:{
                //printf("PC AT PUSH = %d.\n",vm->PC);
                strcpy(vm->vm_stack.data[vm->vm_stack.top].data , instr.op1);

                if(instr.op1[0] == 'L' && isdigit(instr.op1[1])){
                    vm->vm_stack.data[vm->vm_stack.top].is_label = 1;
                }
                else{
                    vm->vm_stack.data[vm->vm_stack.top].is_label = 0;
                }
                
                vm->vm_stack.top++;
                break;
            }

            case TAC_POP:{
                //printf("PC AT POP = %d\n",vm->PC);
                vm->vm_stack.top--;
                float pop_val = get_name(vm->vm_stack.data[vm->vm_stack.top].data , vm);
                set_name(instr.result , pop_val , vm);
                break;
            }

            case TAC_JMP_DYNAMIC:{
                //printf("PC AT JUMP DYNAMIC  = %d.\n",vm->PC);
                vm->vm_stack.top--;
                char value[50];
                strcpy(value , vm->vm_stack.data[vm->vm_stack.top].data);
                vm->PC = find_label(value , vm);
                continue;
            }

            default:
                //printf("Unknown Instruction type %d at vm->PC = %d.\n",instr.type , vm->PC);
                break;
        }
        vm->PC++;
        //printf("stack top = %d\n",vm->vm_stack.top);
        //printf("vm->PC:%d\n",vm->PC);
        if(vm->PC<0){
            break;
        }
    }
}

DataType find_type(char *name , VM *vm){
    //printf("the name is : %s\n",name);
    //printf("sym count = %d.\n",vm->symbol.sym_count);
    for(int i=0 ; i<vm->symbol.sym_count ; i++){
        //printf("symbol = %s.\n",vm->symbol.table[i].sym);
        if(strcmp(vm->symbol.table[i].sym , name) == 0){
            return vm->symbol.table[i].type;
        }
    }
    return UNKNOWN_TYPE;
}

void append_vm(char *buffer , char *line){
    strcat(buffer , line);
}

void BUILD_VM_TEXT(VM *vm , char *buffer){
    buffer[0] = '\0';
    char line[200];

    sprintf(line , "\n-----VM_MEMORY_STATE-----\n");
    append_vm(buffer , line);

    for(int i=0 ; i<vm->memory.memory_count ; i++){
        if(vm->memory.data[i].name[0] == 't' && isdigit(vm->memory.data[i].name[1])){
            continue;
        }
        if(strcmp(vm->memory.data[i].name , "RETVAL") == 0){
            continue;
        }

        DataType type = find_type(vm->memory.data[i].name , vm);

        if(type == TYPE_INT){
            sprintf(line , "%3s = %d\n",vm->memory.data[i].name , (int)vm->memory.data[i].value);
            append_vm(buffer , line);
        }
        else if(type == TYPE_FLOAT){
            sprintf(line , "%3s = %f\n",vm->memory.data[i].name , vm->memory.data[i].value);
            append_vm(buffer , line);
        }
        else if(type == TYPE_CHAR){
            sprintf(line , "%3s = %c\n",vm->memory.data[i].name , (char)vm->memory.data[i].value);
            append_vm(buffer , line);
        }
        else if(type == TYPE_VOID){
            sprintf(line , "%3s = %c\n",vm->memory.data[i].name , (char)vm->memory.data[i].value);
            append_vm(buffer , line);
        }
        else{
            sprintf(line , "error , unknown type.\n");
        }
    }
    sprintf(line , "RET_VAL = %f\n",vm->RET_VAL);
    append_vm(buffer , line);

}

const char *VM_TEXT(char *buffer){
    return buffer;
}