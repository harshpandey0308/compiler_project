#include"TACcode.h"
#include"compiler_result.h"

const char* TAC_text(char *buffer);

float eval_TAC(const char* op1 ,const char* op2 ,const char* opr){
    float a = atof(op1);
    float b = atof(op2);

    if(strcmp(opr,"+")==0)return a+b;
    if(strcmp(opr,"-")==0)return a-b;
    if(strcmp(opr,"*")==0)return a*b;
    if(strcmp(opr,"/")==0)return a/b;

    return 0;
}

void append_TAC(char *buffer , const char* text){
    strcat(buffer , text);
}

void constant_fold(TACProgram *program){
    for(int i=0 ; i<program->tac_count ; i++){
        if(strcmp(program->code[i].op2 , "")==0) continue;

        int op1_const = isdigit(program->code[i].op1[0]);
        int op2_const = isdigit(program->code[i].op2[0]);

        if(op1_const && op2_const){
            float result = eval_TAC(program->code[i].op1 , program->code[i].op2 , program->code[i].opr);

            sprintf(program->code[i].op1,"%g",result);
            strcpy(program->code[i].op2 , "");
            strcpy(program->code[i].opr , "=");

            printf("\nFolded : %s = %g.\n",program->code[i].result , result);
        }
    }
}

void Const_propagate(TACProgram *program){
    for(int i=0 ; i<program->tac_count ; i++){

        if(strcmp(program->code[i].op2,"")==0 && isdigit(program->code[i].op1[0]) && program->code[i].result[0] == 't'){

            char* temp_name = program->code[i].result;
            char* const_value = program->code[i].op1;

            for(int j=i+1 ; j<program->tac_count ; j++){
                if(strcmp(program->code[j].result , temp_name) == 0){
                    break;
                }
                if(strcmp(program->code[j].op1 , temp_name)==0){
                    strcpy(program->code[j].op1 , const_value);
                    printf("Propagated : %s replace with %s in t%d.\n",temp_name , const_value , j+1);
                }
                if(strcmp(program->code[j].op2 , temp_name)==0){
                    strcpy(program->code[j].op2 , const_value);
                    printf("Propagated : %s replace with %s in t%d.\n",temp_name , const_value , j+1);
                }
            }
        }
    }
}




void dead_code(TACProgram *program){
    for(int i=0 ; i<program->tac_count ; i++){

        if(program->code[i].result[0] != 't') continue;

        int is_used = 0;

        for(int j=0 ; j<program->tac_count ; j++){

            if(i==j) continue;

            if(strcmp(program->code[j].op1 , program->code[i].result) == 0 || strcmp(program->code[j].op2 , program->code[i].result)==0){
                printf("t%d is used in t%d.\n",i+1 , j+1);
                is_used = 1;
                break;
            }
        }

        if(!is_used){
            program->code[i].is_dead = 1;
            printf("Dead : %s = %s is never used , Removing!.\n",program->code[i].result , program->code[i].op1);
        }
    }
}

void BUILD_TAC_TEXT(char *buffer ,const TACProgram *program){

    buffer[0] = '\0';

    char line[256];

    sprintf(line , "\n-----------TAC CODE-----------\n");
    //printf("TAC is :\n");
    for(int i=0 ; i<program->tac_count ; i++){
        //printf("checking dead code\n");
        if(program->code[i].is_dead){
            sprintf(line , "%s\n" , "DEAD CODE FOUND.");
            append_TAC(buffer , line);
            continue;
        }

        //printf("tac is printing using switch\n");
        switch(program->code[i].type){
            case TAC_ASSIGN:
                if(strcmp(program->code[i].op2 , "")==0){
                    if(strcmp(program->code[i].opr , "&") == 0 || strcmp(program->code[i].opr , "*") == 0){
                        if(program->code[i].is_deref_write == 1){
                            sprintf(line , "%s %s = %s\n" ,program->code[i].opr , program->code[i].result ,  program->code[i].op1);
                            append_TAC(buffer , line);
                        }
                        else{
                            sprintf(line , "%s = %s %s\n" , program->code[i].result , program->code[i].opr , program->code[i].op1);
                            append_TAC(buffer , line);
                        }
                        
                    }
                    else{
                        sprintf(line , "%s = %s \n" , program->code[i].result , program->code[i].op1);
                        append_TAC(buffer , line);
                    }
                }
                else{
                    sprintf(line , "%s = %s %s %s\n" , program->code[i].result , program->code[i].op1 , program->code[i].opr , program->code[i].op2);
                    append_TAC(buffer , line);
                }
                break;

            case TAC_IF_GOTO:
                //printf("%s , %s , %s , %s\n",program->code[i].op1 , program->code[i].op2 , program->code[i].opr);
                sprintf(line , "IF %s %s %s GOTO %s \n" , program->code[i].op1 , program->code[i].opr , program->code[i].op2 , program->code[i].label);
                append_TAC(buffer , line);
                break;
            
            case TAC_GOTO:
                sprintf(line , "GOTO %s\n" , program->code[i].label);
                append_TAC(buffer , line);
                break;

            case TAC_LABEL:
                sprintf(line , "%s:\n" , program->code[i].label);
                append_TAC(buffer , line);
                break;

            case PARAM:
                sprintf(line , "PARAM %s\n" , program->code[i].op1);
                append_TAC(buffer , line);
                break;
            
            case FUNC_CALL:
                sprintf(line , "CALL %s , %s\n" , program->code[i].op1 , program->code[i].op2);
                append_TAC(buffer , line);
                break;
            
            case RETURN:
                sprintf(line , "RETURN %s\n" , program->code[i].op1);
                append_TAC(buffer , line);
                break;

            case TAC_PUSH:
                sprintf(line , "PUSH %s\n" , program->code[i].op1);
                append_TAC(buffer , line);
                break;
            
            case TAC_POP:
                sprintf(line , "POP %s\n" , program->code[i].result);
                append_TAC(buffer , line);
                break;
            
            case TAC_JMP_DYNAMIC:
                sprintf(line , "JMP [%s]\n" , program->code[i].op1);
                append_TAC(buffer , line);
                break;
            
            case TAC_FUNC_BEGIN:
                sprintf(line , "%s:\n" , program->code[i].label);
                append_TAC(buffer , line);
                break;

            case TAC_PARAM_STRING:
                sprintf(line , "PARAM STRING : %s\n" , program->code[i].op1);
                append_TAC(buffer , line);
                break;

            case TAC_PARAM_ADDR:
                sprintf(line , "PARAM ADDR : %s\n" , program->code[i].op1);
                append_TAC(buffer , line);
                break;
        }   

        
    }
}

const char* TAC_text(char *buffer){
    return buffer;
}

    //void optimizer(){
    //    printf("Optimizer started\n");
    //    print_TAC();

    //    constant_fold();
    //    Const_propagate();
    //    dead_code();

    //    printf("After Optimization:\n.");

    //    print_TAC();
    //}