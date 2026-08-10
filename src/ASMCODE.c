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

void append_ASM(char *buffer , const char *line){
    strcat(buffer , line);
}

void Generate_code(TACProgram *program , REGISTER *registers , char *buffer){
    
    buffer[0] = '\0';

    char line[256];
    sprintf(line , "\n-----------ASSEMBLY CODE--------------\n");
    append_ASM(buffer , line);

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
                    sprintf(line , "MOV %s, %s\n", result , regs_name[reg]);
                    append_ASM(buffer , line);
                    free_reg(reg , registers);
                }
                else if(program->code[i].is_addr == 1 && strcmp(program->code[i].opr , "&") == 0){
                    //printf("else if for addr.\n");
                    sprintf(line , "MOV %s , &%s\n",result , op1);
                    append_ASM(buffer , line);
                    free_reg(reg , registers);
                }
                else if(program->code[i].is_deref_write == 1 && strcmp(program->code[i].opr , "*") == 0){
                    //printf("else if\n");
                    sprintf(line , "MOV *%s , ",result);
                    append_ASM(buffer , line);
                    sprintf(line , "%s\n",op1);
                    append_ASM(buffer , line);
                }
                else{
                    //printf("else\n");
                    sprintf(line , "MOV %s, %s\n",result , op1);
                    append_ASM(buffer , line);
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
                sprintf(line , "MOV %s, %s\n",regs_name[reg] , regs_name[op1_reg]);
                append_ASM(buffer , line);
                free_reg(op1_reg , registers);
            }else{
                sprintf(line , "MOV %s, %s\n",regs_name[reg] , op1);
                append_ASM(buffer , line);
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
                sprintf(line , "%s %s, %s\n",instr , regs_name[reg] , regs_name[op2_reg]);
                append_ASM(buffer , line);
                free_reg(op2_reg , registers);
            }else{
                sprintf(line , "%s %s, %s\n",instr , regs_name[reg] , op2);
                append_ASM(buffer , line);
            }

            break;
            }
            case TAC_IF_GOTO:
                sprintf(line , "CMP %s , %s\n" , program->code[i].op1 , program->code[i].op2);
                append_ASM(buffer , line);

                if(strcmp(program->code[i].opr , "<") == 0){
                    sprintf(line , "JL %s \n", program->code[i].label);
                    append_ASM(buffer , line);
                }
                else if(strcmp(program->code[i].opr , ">") == 0){
                    sprintf(line , "JG %s \n", program->code[i].label);
                    append_ASM(buffer , line);
                }
                else if(strcmp(program->code[i].opr , "==") == 0){
                    sprintf(line , "JE %s \n", program->code[i].label);
                    append_ASM(buffer , line);
                }
                else if(strcmp(program->code[i].opr , "!=") == 0){
                    sprintf(line , "JNE %s \n", program->code[i].label);
                    append_ASM(buffer , line);
                }
                else if(strcmp(program->code[i].opr , "<=") == 0){
                    sprintf(line , "JLE %s \n", program->code[i].label);
                    append_ASM(buffer , line);
                }
                else if(strcmp(program->code[i].opr , ">=") == 0){
                    sprintf(line , "JGE %s \n", program->code[i].label);
                    append_ASM(buffer , line);
                }
                break;
            case TAC_GOTO:
                sprintf(line , "JMP %s \n",program->code[i].label);
                append_ASM(buffer , line);
                break;
            case TAC_LABEL:
                sprintf(line , "%s:\n", program->code[i].label);
                append_ASM(buffer , line);
                break;

            case PARAM:
                sprintf(line , "PUSH %s\n",program->code[i].op1);
                append_ASM(buffer , line);
                break;

            case FUNC_CALL:
                sprintf(line , "CALL %s\n",program->code[i].op1);
                append_ASM(buffer  ,line);
                sprintf(line , "MOV RETVAL , R0\n");
                append_ASM(buffer , line);
                break;

            case RETURN:
                sprintf(line , "MOV R0 , %s\n",program->code[i].op1);
                append_ASM(buffer , line);
                sprintf(line , "RET\n");
                append_ASM(buffer , line);
                break;
        }
    }
}