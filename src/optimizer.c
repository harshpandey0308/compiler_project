#include"TACcode.h"


float eval_TAC(char* op1 , char* op2 , char* opr){
    float a = atof(op1);
    float b = atof(op2);

    if(strcmp(opr,"+")==0)return a+b;
    if(strcmp(opr,"-")==0)return a-b;
    if(strcmp(opr,"*")==0)return a*b;
    if(strcmp(opr,"/")==0)return a/b;

    return 0;
}

void constant_fold(){
    for(int i=0 ; i<tac_count ; i++){
        if(strcmp(tac_table[i].op2 , "")==0) continue;

        int op1_const = isdigit(tac_table[i].op1[0]);
        int op2_const = isdigit(tac_table[i].op2[0]);

        if(op1_const && op2_const){
            float result = eval_TAC(tac_table[i].op1 , tac_table[i].op2 , tac_table[i].opr);

            sprintf(tac_table[i].op1,"%g",result);
            strcpy(tac_table[i].op2 , "");
            strcpy(tac_table[i].opr , "=");

            printf("\nFolded : %s = %g.\n",tac_table[i].result , result);
        }
    }
}

void Const_propagate(){
    for(int i=0 ; i<tac_count ; i++){

        if(strcmp(tac_table[i].op2,"")==0 && isdigit(tac_table[i].op1[0]) && tac_table[i].result[0] == 't'){

            char* temp_name = tac_table[i].result;
            char* const_value = tac_table[i].op1;

            for(int j=i+1 ; j<tac_count ; j++){
                if(strcmp(tac_table[j].result , temp_name) == 0){
                    break;
                }
                if(strcmp(tac_table[j].op1 , temp_name)==0){
                    strcpy(tac_table[j].op1 , const_value);
                    printf("Propagated : %s replace with %s in t%d.\n",temp_name , const_value , j+1);
                }
                if(strcmp(tac_table[j].op2 , temp_name)==0){
                    strcpy(tac_table[j].op2 , const_value);
                    printf("Propagated : %s replace with %s in t%d.\n",temp_name , const_value , j+1);
                }
            }
        }
    }
}




void dead_code(){
    for(int i=0 ; i<tac_count ; i++){

        if(tac_table[i].result[0] != 't') continue;

        int is_used = 0;

        for(int j=0 ; j<tac_count ; j++){

            if(i==j) continue;

            if(strcmp(tac_table[j].op1 , tac_table[i].result) == 0 || strcmp(tac_table[j].op2 , tac_table[i].result)==0){
                printf("t%d is used in t%d.\n",i+1 , j+1);
                is_used = 1;
                break;
            }
        }

        if(!is_used){
            tac_table[i].is_dead = 1;
            printf("Dead : %s = %s is never used , Removing!.\n",tac_table[i].result , tac_table[i].op1);
        }
    }
}

void print_TAC(){
    printf("\n-----------TAC CODE-----------\n");
    //printf("TAC is :\n");
    for(int i=0 ; i<tac_count ; i++){
        //printf("checking dead code\n");
        if(tac_table[i].is_dead){
            printf("dead code found");
            continue;}

        //printf("tac is printing using switch\n");
        switch(tac_table[i].type){
            case TAC_ASSIGN:
                if(strcmp(tac_table[i].op2 , "")==0){
                    if(strcmp(tac_table[i].opr , "&") == 0 || strcmp(tac_table[i].opr , "*") == 0){
                        if(tac_table[i].is_deref_write == 1){
                            printf("%s %s = %s\n",tac_table[i].opr , tac_table[i].result ,  tac_table[i].op1);
                        }
                        else{
                            printf("%s = %s %s\n",tac_table[i].result , tac_table[i].opr , tac_table[i].op1);
                        }
                        
                    }
                    else{
                        printf("%s = %s \n",tac_table[i].result , tac_table[i].op1);
                    }
                }
                else{
                    printf("%s = %s %s %s\n",tac_table[i].result , tac_table[i].op1 , tac_table[i].opr , tac_table[i].op2);
                }
                break;

            case TAC_IF_GOTO:
                //printf("%s , %s , %s , %s\n",tac_table[i].op1 , tac_table[i].op2 , tac_table[i].opr);
                printf("IF %s %s %s GOTO %s \n",tac_table[i].op1 , tac_table[i].opr , tac_table[i].op2 , tac_table[i].label);
                break;
            
            case TAC_GOTO:
                printf("GOTO %s\n",tac_table[i].label);
                break;

            case TAC_LABEL:
                printf("%s:\n",tac_table[i].label);
                break;

            case PARAM:
                printf("PARAM %s\n",tac_table[i].op1);
                break;
            
            case FUNC_CALL:
                printf("CALL %s , %s\n",tac_table[i].op1 , tac_table[i].op2);
                break;
            
            case RETURN:
                printf("RETURN %s\n", tac_table[i].op1);
                break;

            case TAC_PUSH:
                printf("PUSH %s\n",tac_table[i].op1);
                break;
            
            case TAC_POP:
                printf("POP %s\n",tac_table[i].result);
                break;
            
            case TAC_JMP_DYNAMIC:
                printf("JMP [%S]\n",tac_table[i].op1);
                break;
            
            case TAC_FUNC_BEGIN:
                printf("%s:\n",tac_table[i].label);
                break;

            case TAC_PARAM_STRING:
                printf("PARAM STRING : %s\n",tac_table[i].op1);
                break;

            case TAC_PARAM_ADDR:
                printf("PARAM ADDR : %s\n",tac_table[i].op1);
                break;
        }   

        
    }
}

void optimizer(){
    printf("Optimizer started\n");
    print_TAC();

    constant_fold();
    Const_propagate();
    dead_code();

    printf("After Optimization:\n.");

    print_TAC();
}