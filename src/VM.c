#include<stdio.h>
#include<stdbool.h>
#include"TACcode.h"
#include"semantic.h"
#include"VM.h"
#include"compiler.h"

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

MEMBER get_name(char *val , VM *vm){
    MEMBER member = {0};
    member.active_type = UNKNOWN_TYPE;

    if(val == NULL){
        return member;
    }

    if(isdigit((unsigned char) val[0]) || val[0] =='-'){
        member.active_type = FLOAT;
        member.VALUE.float_number = atof(val);
        return member;
    }

    if(strcmp(val , "RETVAL") == 0){
        member.active_type = FLOAT;
        member.VALUE.float_number = vm->RET_VAL;
        return member;
    }

    for(int i=0 ; i<vm->memory.memory_count ; i++){
        if(strcmp(vm->memory.data[i].name , val) == 0){
            member.active_type = vm->memory.data[i].type;
            if(member.active_type == INTEGER){
                member.VALUE.integer = vm->memory.data[i].value.int_val;
            }
            else if(member.active_type == FLOAT){
                member.VALUE.float_number = vm->memory.data[i].value.float_val;
            }
            else if(member.active_type == CHARACTER){
                member.VALUE.character = vm->memory.data[i].value.ch_val;
            }
            return member;
        }
    }
    return member;
}

void set_name(char *name , MEMBER *member , VM *vm){
    for(int i=0 ; i<vm->memory.memory_count ; i++){
        if(strcmp(vm->memory.data[i].name , name) == 0){
            //printf("Setting value of %s to %f at index %d\n", name, value, i);
            if(member->active_type == INTEGER){
                vm->memory.data[i].type = member->active_type;
                vm->memory.data[i].value.int_val = member->VALUE.integer;
            }
            else if(member->active_type == FLOAT){
                vm->memory.data[i].type = member->active_type;
                vm->memory.data[i].value.float_val = member->VALUE.float_number;
            }
            else if(member->active_type == CHARACTER){
                vm->memory.data[i].type = member->active_type;
                vm->memory.data[i].value.ch_val = member->VALUE.character;
            }

            return;
        }
        
    }
    strcpy(vm->memory.data[vm->memory.memory_count].name , name);
    if(member->active_type == INTEGER){
        vm->memory.data[vm->memory.memory_count].type = member->active_type;
        vm->memory.data[vm->memory.memory_count].value.int_val = member->VALUE.integer;
    }
    else if(member->active_type == FLOAT){
        vm->memory.data[vm->memory.memory_count].type = member->active_type;
        vm->memory.data[vm->memory.memory_count].value.float_val = member->VALUE.float_number;
    }
    else if(member->active_type == CHARACTER){
        vm->memory.data[vm->memory.memory_count].type = member->active_type;
        vm->memory.data[vm->memory.memory_count].value.ch_val = member->VALUE.character;
    }
    //printf("Adding new variable %s with value %f to memory at index %d\n", name, value, vm->memory.memory_count);
    vm->memory.memory_count++;
    return;

}

void set_by_index(int addr , MEMBER *member , VM *vm){
    if(addr >= 0 && addr < vm->memory.memory_count){
        if(member->active_type == INTEGER){
            vm->memory.data[addr].type = member->active_type;
            vm->memory.data[addr].value.int_val = member->VALUE.integer;
        }
        else if(member->active_type == FLOAT){
            vm->memory.data[addr].type = member->active_type;
            vm->memory.data[addr].value.float_val = member->VALUE.float_number;
        }
        else if(member->active_type == CHARACTER){
            vm->memory.data[addr].type = member->active_type;
            vm->memory.data[addr].value.ch_val = member->VALUE.character;
        }
    }
    else{
        printf("Invalid memory address: %d\n", addr);
    }
}

void append_output(char *buffer , char *line){
    strcat(buffer , line);
}

float member_to_float(MEMBER *member){
    if(member->active_type == INTEGER){
        return member->VALUE.integer;
    }
    else if(member->active_type == FLOAT){
        return member->VALUE.float_number;
    }
    return 0.0f;
}

void handle_printf(int arg_count , VM *vm , char *buffer){

    char line[256];

    char ret_value[50];

    //printf("[PRINTF] , stack tope before = %d.\n",vm->vm_stack.top);

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
        //printf("DEBUG arg_arr[%d]='%s'\n",i , arg_arr[i]);
        //printf("%d\n",i);
        if(fmt[i] == '%'){
            i++;
            if(fmt[i] == 'd'){
                int arg = (int)atof(arg_arr[arg_indx++]);
                sprintf(line , "%d",arg);
                append_output(buffer , line);
            }
            else if(fmt[i] == 'f'){
                float arg1 = atof(arg_arr[arg_indx++]);
                sprintf(line , "%f",arg1);
                append_output(buffer , line);
            }
            else if(fmt[i] == 'c'){
                //printf("value is %f.\n",arg_arr[arg_indx]);
                //printf("index = %d\n", arg_indx);
                char arg2 = (char)atof(arg_arr[arg_indx++]);
                sprintf(line , "%c",arg2);
                append_output(buffer , line);
            }
            else if(fmt[i] == 's'){
                sprintf(line , "%s",arg_arr[arg_indx++]);
                append_output(buffer , line);
            }
        }
        else if(fmt[i] == '\\'){
            i++;
            if(fmt[i] == 'n'){
                sprintf(line , "\n");
                append_output(buffer , line);
            }
            else if(fmt[i] == 't'){
                sprintf(line , "\t");
                append_output(buffer , line);
            }
        }
        else{
            //printf("ab\n");
            sprintf(line , "%c",fmt[i]);
            append_output(buffer , line);
        }
    }

    
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
                vm->memory.data[addr].value.float_val = i_value;
                //printf("value : %f\n",memory->data[addr].value);
            }
            else if(fmt[i] == 'f'){
                float f_value;
                scanf("%f",&f_value);
                vm->memory.data[addr].value.float_val = f_value;
            }
            else if(fmt[i] == 'c'){
                char c_val;
                scanf("%c",&c_val);
                vm->memory.data[addr].value.ch_val = c_val;
            }
            //printf("scanf completed.\n");
        }
        
    }

    

    vm->PC = find_label(ret_label , vm);
    //printf("After scanf, vm_stack->top=%d\n", vm_stack->top);
}

void run_vm(VM *vm , CompilerResult *result){
    vm->PC = find_label("main" , vm);

    vm->member.VALUE.character = 0;
    vm->member.VALUE.float_number = 0.0;
    vm->member.VALUE.integer = 0;

    result->output_buffer[0] = '\0';
    //printf("TAC_COUNT:%d\n",vm->program.tac_count);
    while(vm->PC < vm->program.tac_count && vm->vm_stack.top >= 0){
        TAC instr = vm->program.code[vm->PC];
        //printf("instr.type = %d\n",instr.type);
        switch (instr.type){
            case TAC_ASSIGN:{
                printf("[VM] %s = %s %s %s\n",instr.result,instr.op1,instr.opr,instr.op2);

                printf("[VM] sum = %f\n",get_name("sum", vm));

                printf("[ASSIGN] %s = %f\n",instr.result,get_name(instr.op1, vm));
                
                if(instr.is_deref_write == 1){
                    //printf("dereference write operation detected for %s = %s %s \n",instr.result , instr.op1 , instr.opr);
                    vm->member = get_name(instr.result , vm);
                    int addr1 = vm->member.VALUE.integer;
                    vm->member = get_name(instr.op1 , vm);
                    //printf("the right side value = %f\n",val1);
                    set_by_index(addr1 , &vm->member , vm);
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
                                vm->member.VALUE.integer = addr;
                                vm->member.active_type = INTEGER;
                                //printf("addr of %s is %d\n",instr.op1 , addr);
                                break;
                            }
                        }
                        set_name(instr.result , &vm->member , vm);
                    }
                    else if(strcmp(instr.opr , "*") == 0){
                        int ptr_val;
                        //printf("dereference read operation detected for %s = %s\n",instr.result , instr.op1);
                        for(int j=0 ; j<vm->memory.memory_count ; j++){
                            
                            if(strcmp(vm->memory.data[j].name , instr.op1) == 0){
                                int val = (int)vm->memory.data[j].value.int_val;
                                
                                ptr_val = vm->memory.data[val].value.int_val;
                                vm->member.VALUE.integer = ptr_val;
                                vm->member.active_type = INTEGER;
                                //printf("value at addr %d is %f\n",val , ptr_val);
                                break;
                            }
                        }
                        set_name(instr.result , &vm->member , vm);
                    }
                    else{
                        float val;
                        //int n = strlen(instr.op1);
                    
                        vm->member = get_name(instr.op1 , vm);
                        
                        set_name(instr.result , &vm->member , vm);
                        //printf("TAC_ASSIGN: %f\n",val);
                    }
                }

                else if(vm->symbol.table[vm->PC].size > 0){
                    for(int i=0 ; i<vm->symbol.table[vm->PC].size ; i++){
                        char arr_slot[50];
                        sprintf(arr_slot , "%s%d" , vm->symbol.table[i].sym , i);
                        vm->member.VALUE.float_number = 0.0;
                        vm->member.active_type = FLOAT;
                        set_name(arr_slot , &vm->member , vm);
                    }

                }

                else{
                    MEMBER m1 = get_name(instr.op1 , vm);
                    MEMBER m2 = get_name(instr.op2 , vm);

                    float op1_val = member_to_float(&m1);
                    float op2_val = member_to_float(&m2);
                    
                    if(strcmp(instr.opr , "+") == 0){
                        vm->member.VALUE.float_number = op1_val + op2_val;
                        vm->member.active_type = FLOAT;
                        set_name(instr.result , &vm->member , vm);
                    }
                    else if(strcmp(instr.opr , "-") == 0){
                        vm->member.VALUE.float_number = op1_val - op2_val;
                        vm->member.active_type = FLOAT;
                        set_name(instr.result , &vm->member , vm);
                    }
                    else if(strcmp(instr.opr , "*") == 0){
                        vm->member.VALUE.integer = op1_val*op2_val;
                        
                        vm->member.active_type = INTEGER;
                        set_name(instr.result , &vm->member , vm);
                    } 
                    else if(strcmp(instr.opr , "%") == 0){
                        int result1 = 0;
                        result1 = (int)(op1_val)%(int)(op2_val);
                        vm->member.VALUE.integer = result1;
                        
                        vm->member.active_type = INTEGER;
                        set_name(instr.result , &vm->member , vm);
                    } 
                    else if(strcmp(instr.opr , "/") == 0){
                        int result2 = 0;
                        result2 = op1_val/op2_val;
                        vm->member.VALUE.integer = result2;
                        
                        vm->member.active_type = INTEGER;
                        set_name(instr.result , &vm->member , vm);
                    }
                    
                    //printf("[ARITH] %s = %f %s %f => %f\n",instr.result,op1_val,instr.opr,op2_val,result1);
                    //printf("TAC_ASSIGN: %f\n", result);
                }
                
                break;
            }
            
            case TAC_IF_GOTO:{
                
                float val1;
                float val2;
                float ptr_val1;
                float ptr_val2;
                int cond = 0;
                
                if(instr.op1[0] == '*' && isalpha(instr.op1[1]) && isdigit(instr.op2[0])){
                    char ptr_name[50];
                    strcpy(ptr_name , instr.op1 + 1);
                    vm->member = get_name(ptr_name , vm);
                    int p_index = member_to_float(&vm->member);
                    ptr_val1 = vm->memory.data[p_index].value.float_val;
                    vm->member = get_name(instr.op2 , vm);
                    int ptr_val2 = member_to_float(&vm->member);
                    //printf("ptr_val1 = %s , ptr_val2 = %f.\n",ptr_val1 , ptr_val2);
                    if(strcmp(instr.opr , "<") == 0) cond = (ptr_val1 < ptr_val2);
                    else if(strcmp(instr.opr , ">") == 0) cond = (ptr_val1 > ptr_val2);
                    else if(strcmp(instr.opr , "<=") == 0) cond = (ptr_val1 <= ptr_val2);
                    else if(strcmp(instr.opr , ">=") == 0) cond = (ptr_val1 >= ptr_val2);
                    else if(strcmp(instr.opr , "==") == 0) cond = (ptr_val1 == ptr_val2);
                    else if(strcmp(instr.opr , "!=") == 0) cond = (ptr_val1 != ptr_val2);
                }
                else{
                    vm->member = get_name(instr.op1 , vm);
                    float val1 = member_to_float(&vm->member);
                    vm->member = get_name(instr.op2 , vm);
                    float val2 = member_to_float(&vm->member);
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
                
                int ind = find_label(instr.label , vm);
                //printf("TAC_GOTO: %d\n",ind);
                vm->PC = ind;
                continue;
            }

            case PARAM:{
                MEMBER m1 = get_name(instr.op1 , vm);
                float op1_value = member_to_float(&m1);
                //printf("vm_stack->top AT PARAM : %d\n", vm_stack->top);
                sprintf(vm->vm_stack.data[vm->vm_stack.top].data , "%f" , op1_value);
                vm->vm_stack.data[vm->vm_stack.top].is_label = 0;
                vm->vm_stack.top++;
                
                break;
            }
            
            case FUNC_CALL:{
                //printf("PC AT FUNCTION CALL = %d.\n",vm->PC);
                char *func_name = instr.op1;
                //printf("func name = %s\n",func_name);
                int param_count = 0;

                int arg_count = atoi(instr.op2);

                if(strcmp(func_name , "printf") == 0){
                    handle_printf(arg_count , vm , result->output_buffer);
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
                
                char ret_label[50];
                vm->vm_stack.top--;
                if(vm->vm_stack.top<0){
                    break;
                }
                strcpy(ret_label , vm->vm_stack.data[vm->vm_stack.top].data);
                
                float args[10] ;
                for(int i=param_count-1 ; i>=0 ;i--){
                    vm->vm_stack.top--;
                    args[i] = atof(vm->vm_stack.data[vm->vm_stack.top].data);
                    if(vm->vm_stack.top<0){
                        
                        break;
                    }
                }
                

                int param_index = 0 ;
                
                for(int j=0 ; j<vm->symbol.sym_count ; j++){
                    //printf("i = %d.\n",vm->symbol.sym_count);
                    if(vm->symbol.table[j].is_param == 1 && strcmp(vm->symbol.table[j].scope , func_name) == 0){
                        vm->member.VALUE.float_number = args[param_index];
                        vm->member.active_type = FLOAT;
                        set_name(vm->symbol.table[j].sym , &vm->member , vm);
                        param_index++;
                    }
                }

                
                strcpy(vm->vm_stack.data[vm->vm_stack.top].data , ret_label);
                
                vm->vm_stack.data[vm->vm_stack.top].is_label = 1;
                vm->vm_stack.top++;
                

                vm->PC = find_label(func_name , vm);
                
                continue;
            }

            case TAC_PARAM_STRING:{
                
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
                    vm->member.VALUE.float_number = 0.0;
                    vm->member.active_type = FLOAT;
                    set_name(instr.op1 , &vm->member , vm);
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
                
                MEMBER m1 = get_name(instr.op1 , vm);
                float value1 = member_to_float(&m1);
                vm->RET_VAL = value1;

                vm->vm_stack.top--;
                char ret_lab[50];
                strcpy(ret_lab , vm->vm_stack.data[vm->vm_stack.top].data);

                int index = find_label(ret_lab , vm);
                if(index < 0){
                    break;
                }

                vm->PC = index;
                
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
                
                vm->vm_stack.top--;
                MEMBER m1 = get_name(vm->vm_stack.data[vm->vm_stack.top].data , vm);
                float pop_val = member_to_float(&m1);
                vm->member.VALUE.float_number = pop_val;
                vm->member.active_type = FLOAT;
                set_name(instr.result , &vm->member , vm);
                break;
            }

            case TAC_JMP_DYNAMIC:{
                
                vm->vm_stack.top--;
                char value[50];
                strcpy(value , vm->vm_stack.data[vm->vm_stack.top].data);
                int index = find_label(value , vm);
                if(index < 0){
                    break;
                }

                vm->PC = index;
                continue;
            }

            default:
                
                break;
        }
        vm->PC++;
        
        
    }
}

DataType find_type(char *name , VM *vm){
    for(int i=0 ; i<vm->symbol.sym_count ; i++){
        
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
    printf("\n-----VM_MEMORY_STATE-----\n");
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
            printf("%3s = %d\n",vm->memory.data[i].name , (int)vm->memory.data[i].value.float_val);
            sprintf(line , "%3s = %d\n",vm->memory.data[i].name , (int)vm->memory.data[i].value.float_val);
            append_vm(buffer , line);
        }
        else if(type == TYPE_FLOAT){
            printf("%3s = %f\n",vm->memory.data[i].name , vm->memory.data[i].value);
            sprintf(line , "%3s = %f\n",vm->memory.data[i].name , vm->memory.data[i].value);
            append_vm(buffer , line);
        }
        else if(type == TYPE_CHAR){
            printf("%3s = %c\n",vm->memory.data[i].name , (char)vm->memory.data[i].value.float_val);
            sprintf(line , "%3s = %c\n",vm->memory.data[i].name , (char)vm->memory.data[i].value.float_val);
            append_vm(buffer , line);
        }
        else if(type == TYPE_VOID){
            printf("%3s = %c\n",vm->memory.data[i].name , (char)vm->memory.data[i].value.float_val);
            sprintf(line , "%3s = %c\n",vm->memory.data[i].name , (char)vm->memory.data[i].value.float_val);
            append_vm(buffer , line);
        }
        else{
            sprintf(line , "error , unknown type.\n");
        }
    }
    sprintf(line , "RET_VAL = %f\n",vm->RET_VAL);
    append_vm(buffer , line);

}

const char *output_text(char *buffer){
    return buffer;
}

const char *VM_TEXT(char *buffer){
    return buffer;
}