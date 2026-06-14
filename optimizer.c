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
    for(int i=0 ; i<tac_count ; i++){
        if(tac_table[i].is_dead) continue;

        switch(tac_table[i].type){
            case TAC_ASSIGN:
                if(strcmp(tac_table[i].op2 , "")==0){
                    printf("%s = %s\n",tac_table[i].result , tac_table[i].op1);
            }
            else{
                printf("%s = %s %s %s\n",tac_table[i].result , tac_table[i].op1 , tac_table[i].opr , tac_table[i].op2);
            }
            break;

            case TAC_IF_GOTO:
                printf("IF %s %s %s GOTO %s \n",tac_table[i].op1 , tac_table[i].opr , tac_table[i].op2 , tac_table[i].label);
                break;
            
            case TAC_GOTO:
                printf("GOTO %s\n",tac_table[i].label);
                break;

            case TAC_LABEL:
                printf("%s:\n",tac_table[i].label);
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