#include"TACcode.h"
#include"ASMCODE.h"

#define MAX_INSTRUCTION_LENGTH 20

const char* regs_name[] = {"R0" , "R1" , "R2" , "R3"};

void stack_push(ASM_STACK *stack , char *value){
    strcpy(stack->STACK[stack->top] , value);
    stack->top++;
}

char *stack_pop(ASM_STACK *stack){
    stack->top--;
    return stack->STACK[stack->top];
}


int alloc_reg(REGISTER *registers){
    for(int i=0 ; i<REG_COUNT ; i++){
        if(registers->reg_free[i] == 1){
            registers->reg_free[i] = 0;
            return i;
        }
    }
    //printf("ERROR!, no free register.\n");
    return -1;
}

void free_reg(int index , REGISTER *registers){
    registers->reg_free[index] = 1;
    for(int i=0 ; i<registers->register_count ; i++){
        if(registers->register_map[i].register_index == index){
            registers->register_map[i].temp[0] = '\0';
            registers->register_map[i].register_index = -1;
            break;
        }
    }
}

int find_reg(const char* temp , const REGISTER *registers){
    for(int i=0 ; i<registers->register_count ; i++){
        if(strcmp(registers->register_map[i].temp , temp)==0){
            return registers->register_map[i].register_index;
        }
    }
    return -1;
}

void Generate_code(TACProgram *program , REGISTER *registers){
    printf("\n-----------ASSEMBLY CODE--------------\n");

    for(int i=0 ; i<program->tac_count ; i++){
        if(program->code[i].is_dead) continue;

        switch(program->code[i].type){
            case TAC_ASSIGN: {
                char* result = program->code[i].result;
                char* op1 = program->code[i].op1;
                char* op2 = program->code[i].op2;
                char* opr = program->code[i].opr;

                //printf("RESULT FOR ASM = %s\n",result);
                //printf("OP1 FOR ASM = %s\n",op1);
                //printf("OP2 FOR ASM = %s\n",op2);
                //printf("OPR FOR ASM = %s\n",opr);
                //printf("DEREFERENCE WRITE = %d\n",program->code[i].is_deref_write);
                //printf("addr of = %d\n",program->code[i].is_addr);
                if(strcmp(op2 , "")==0){
                int reg = find_reg(op1 , registers);
                if(reg != -1 && isalpha(program->code[i].op1[0])){
                    //printf("if.\n");
                    printf("MOV %s, %s\n", result , regs_name[reg]);
                    free_reg(reg , registers);
                }
                else if(program->code[i].is_addr == 1 && strcmp(program->code[i].opr , "&") == 0){
                    //printf("else if for addr.\n");
                    printf("MOV %s , &%s\n",result , op1);
                    free_reg(reg , registers);
                }
                else if(program->code[i].is_deref_write == 1 && strcmp(program->code[i].opr , "*") == 0){
                    //printf("else if\n");
                    printf("MOV *%s , ",result);
                    printf("%s\n",op1);
                }
                else{
                    //printf("else\n");
                    printf("MOV %s, %s\n",result , op1);
                }
                continue;
            }

            int reg = alloc_reg(registers);

            if(reg == -1){
                printf("ERROR , register can not be allocated.\n");
                return;
            }

            strcpy(registers->register_map[registers->register_count].temp , result);
            registers->register_map[registers->register_count].register_index = reg;
            registers->register_count++;

            int op1_reg = find_reg(op1 , registers);

            if(op1_reg != -1){
                printf("MOV %s, %s\n",regs_name[reg] , regs_name[op1_reg]);
                free_reg(op1_reg , registers);
            }else{
                printf("MOV %s, %s\n",regs_name[reg] , op1);
            }

            char instr[MAX_INSTRUCTION_LENGTH];
            if(strcmp(opr , "+")==0) strcpy(instr , "ADD");
            else if(strcmp(opr , "-")==0) strcpy(instr , "SUB");
            else if(strcmp(opr , "*")==0) strcpy(instr , "MUL");
            else if(strcmp(opr , "/")==0) strcpy(instr , "DIV");
            else if(strcmp(opr , "%")==0) strcpy(instr , "MOD");
            else if(strcmp(opr , "=")==0) strcpy(instr , "MOV");
        
            int op2_reg = find_reg(op2 , registers);

            if(op2_reg != -1){
                printf("%s %s, %s\n",instr , regs_name[reg] , regs_name[op2_reg]);
                free_reg(op2_reg , registers);
            }else{
                printf("%s %s, %s\n",instr , regs_name[reg] , op2);
            }

            break;
            }
            case TAC_IF_GOTO:
                printf("CMP %s , %s\n" , program->code[i].op1 , program->code[i].op2);

                if(strcmp(program->code[i].opr , "<") == 0){
                    printf("JL %s \n", program->code[i].label);
                }
                else if(strcmp(program->code[i].opr , ">") == 0){
                    printf("JG %s \n", program->code[i].label);
                }
                else if(strcmp(program->code[i].opr , "==") == 0){
                    printf("JE %s \n", program->code[i].label);
                }
                else if(strcmp(program->code[i].opr , "!=") == 0){
                    printf("JNE %s \n", program->code[i].label);
                }
                else if(strcmp(program->code[i].opr , "<=") == 0){
                    printf("JLE %s \n", program->code[i].label);
                }
                else if(strcmp(program->code[i].opr , ">=") == 0){
                    printf("JGE %s \n", program->code[i].label);
                }
                break;
            case TAC_GOTO:
                printf("JMP %s \n",program->code[i].label);
                break;
            case TAC_LABEL:
                printf("%s:\n", program->code[i].label);
                break;

            case PARAM:
                printf("PUSH %s\n",program->code[i].op1);
                break;

            case FUNC_CALL:
                printf("CALL %s\n",program->code[i].op1);
                printf("MOV RETVAL , R0\n");
                break;

            case RETURN:
                printf("MOV R0 , %s\n",program->code[i].op1);
                printf("RET\n");
                break;
        }
    }
}