#include"TACcode.h"
#include"optimizer.h"
#include"ASMCODE.h"

char* SP = "SP";

REG_MAP regmap[100];
int reg_map_count = 0;

const char* reg_name[] = {"R0" , "R1" , "R2" , "R3"};

int reg_free[] = {1,1,1,1};

char STACK[1000][50];
int SPTR = 0;

void stack_push(char *value){
    strcpy(STACK[SPTR] , value);
    SPTR++;
}

char *stack_pop(){
    SPTR--;
    return STACK[SPTR];
}


int alloc_reg(){
    for(int i=0 ; i<REG_COUNT ; i++){
        if(reg_free[i] == 1){
            reg_free[i] = 0;
            return i;
        }
    }
    printf("ERROR!, no free register.\n");
    return -1;
}

void free_reg(int index){
    reg_free[index] = 1;
}

int find_reg(char* temp){
    for(int i=0 ; i<reg_map_count ; i++){
        if(strcmp(regmap[i].temp , temp)==0){
            return regmap[i].reg_index;
        }
    }
    return -1;
}

void Generate_code(){
    printf("\n-----------ASSEMBLY CODE--------------\n");

    for(int i=0 ; i<tac_count ; i++){
        if(tac_table[i].is_dead) continue;

        switch(tac_table[i].type){
            case TAC_ASSIGN: {
                char* result = tac_table[i].result;
                char* op1 = tac_table[i].op1;
                char* op2 = tac_table[i].op2;
                char* opr = tac_table[i].opr;

                if(strcmp(op2 , "")==0){
                int reg = find_reg(op1);
                if(reg != -1){
                    printf("MOV %s, %s\n", result , reg_name[reg]);
                    free_reg(reg);
                }
                else{
                    printf("MOV %s, %s\n",result , op1);
                }
                continue;
            }

            int reg = alloc_reg();

            strcpy(regmap[reg_map_count].temp,result);
            regmap[reg_map_count].reg_index = reg;
            reg_map_count++;

            int op1_reg = find_reg(op1);

            if(op1_reg != -1){
                printf("MOV %s, %s\n",reg_name[reg] , reg_name[op1_reg]);
                free_reg(op1_reg);
            }else{
                printf("MOV %s, %s\n",reg_name[reg] , op1);
            }

            char instr[10];
            if(strcmp(opr , "+")==0) strcpy(instr , "ADD");
            else if(strcmp(opr , "-")==0) strcpy(instr , "SUB");
            else if(strcmp(opr , "*")==0) strcpy(instr , "MUL");
            else if(strcmp(opr , "/")==0) strcpy(instr , "DIV");
            else if(strcmp(opr , "%")==0) strcpy(instr , "MOD");
            else if(strcmp(opr , "=")==0) strcpy(instr , "MOV");
        
            int op2_reg = find_reg(op2);

            if(op2_reg != -1){
                printf("%s %s, %s\n",instr , reg_name[reg] , reg_name[op2_reg]);
                free_reg(op2_reg);
            }else{
                printf("%s %s, %s\n",instr , reg_name[reg] , op2);
            }

            break;
            }
            case TAC_IF_GOTO:
                printf("CMP %s , %s\n" , tac_table[i].op1 , tac_table[i].op2);

                if(strcmp(tac_table[i].opr , "<") == 0){
                    printf("JL %s \n", tac_table[i].label);
                }
                else if(strcmp(tac_table[i].opr , ">") == 0){
                    printf("JG %s \n", tac_table[i].label);
                }
                else if(strcmp(tac_table[i].opr , "==") == 0){
                    printf("JE %s \n", tac_table[i].label);
                }
                else if(strcmp(tac_table[i].opr , "!=") == 0){
                    printf("JNE %s \n", tac_table[i].label);
                }
                else if(strcmp(tac_table[i].opr , "<=") == 0){
                    printf("JLE %s \n", tac_table[i].label);
                }
                else if(strcmp(tac_table[i].opr , ">=") == 0){
                    printf("JGE %s \n", tac_table[i].label);
                }
                break;
            case TAC_GOTO:
                printf("JMP %s \n",tac_table[i].label);
                break;
            case TAC_LABEL:
                printf("%s:\n", tac_table[i].label);
                break;

            case PARAM:
                printf("PUSH %s\n",tac_table[i].op1);
                break;

            case FUNC_CALL:
                printf("CALL %s\n",tac_table[i].op1);
                printf("MOV RETVAL , R0\n");
                break;

            case RETURN:
                printf("MOV R0 , %s\n",tac_table[i].op1);
                printf("RET\n");
                break;
        }
    }
}